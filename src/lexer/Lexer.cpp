// fmt
#include <fmt/core.h>

// vought
#include <lexer/Lexer.hpp>

// std
#include <stack>

namespace Vought {

char const* LexerException::what() const noexcept {
    return mMessage.c_str();
}

LexerBuilder& LexerBuilder::addSource(
    std::string const& source) {
    mSource = source;

    return *this;
}

LexerBuilder& LexerBuilder::addCategory(
    int category, std::function<bool(char)> check) {
    if (category < 0)
        throw LexerException(fmt::format(
            "tried to initialise with negative category {}",
            category));

    mCategories[category] = check;

    return *this;
}

LexerBuilder& LexerBuilder::setInitialState(int state) {
    if (state < 0)
        throw LexerException(fmt::format(
            "tried to set negative initial state {}",
            state));

    mInitialState = state;

    mStates.insert(state);

    return *this;
}

LexerBuilder& LexerBuilder::addTransition(int state,
                                          int category,
                                          int nextState) {
    if (mCategories.count(category) <= 0)
        throw LexerException(
            fmt::format("tried to add a transition using "
                        "an unregistered category {}",
                        category));
    if (state < 0)
        throw LexerException(
            fmt::format("used negative state {}", state));

    if (nextState < 0)
        throw LexerException(fmt::format(
            "used negative nextState {}", nextState));

    mStates.insert({state, nextState});

    mTransitions[{state, category}] = nextState;

    return *this;
}

LexerBuilder& LexerBuilder::setStateAsFinal(
    int state, Token::Type type) {
    if (mStates.count(state) <= 0)
        throw LexerException(
            fmt::format("tried to add a final state using "
                        "an unregistered state {}",
                        state));

    mFinalStates[state] = type;

    return *this;
}

LexerBuilder& LexerBuilder::reset() {
    mStates.clear();
    mCategories.clear();
    mTransitions.clear();
    mInitialState.reset();
    mFinalStates.clear();

    return *this;
}

Lexer LexerBuilder::build() {
    if (!mInitialState.has_value())
        throw LexerException(
            "cannot build a lexer with an initial state");

    int noOfStates = mStates.size();
    int noOfCategories = mCategories.size();
    int noOfFinalStates = mFinalStates.size();

    std::vector<std::vector<int>> transitionTable =
        std::vector<std::vector<int>>(
            noOfStates, std::vector<int>(noOfCategories,
                                         INVALID_STATE));

    // state fields
    int initialStateIndex;

    std::unordered_set<int> finalStateIndices;

    std::unordered_map<int, Token::Type>
        finalStateIndexToTokenType;

    std::unordered_map<int, int> stateToIndex;

    int stateIndex = 0;

    for (auto const& state : mStates) {
        // indexToState.push_back(state);
        stateToIndex[state] = stateIndex;

        if (state == mInitialState)
            initialStateIndex = stateIndex;

        if (mFinalStates.count(state) > 0) {
            finalStateIndices.insert(stateIndex);

            finalStateIndexToTokenType[stateIndex] =
                mFinalStates[state];
        }

        stateIndex++;
    }

    // category fields
    std::unordered_map<int, std::function<bool(char)>>
        categoryIndexToChecker;

    std::unordered_map<int, int> categoryToIndex;

    int categoryIndex = 0;

    for (auto const& [category, type] : mCategories) {
        categoryToIndex[category] = categoryIndex;

        categoryIndexToChecker[categoryIndex] = type;

        categoryIndex++;
    }

    // transition table
    for (auto const& [input, nextState] : mTransitions) {
        auto const& [state, category] = input;

        transitionTable[stateToIndex[state]]
                       [categoryToIndex[category]] =
                           stateToIndex[nextState];
    }

    // create dfsa
    DFSA dfsa;

    dfsa.mNoOfStates = noOfStates;
    dfsa.mNoOfCategories = noOfCategories;
    dfsa.mTransitionTable = std::move(transitionTable);
    dfsa.mInitialState = initialStateIndex;
    dfsa.mFinalStates = std::move(finalStateIndices);

    // create lexer
    Lexer lexer;

    lexer.mSource = std::move(mSource);
    lexer.mDFSA = dfsa;
    lexer.mCategoryToChecker =
        std::move(categoryIndexToChecker);
    lexer.mFinalStateToTokenType =
        std::move(finalStateIndexToTokenType);

    reset();

    return lexer;
}

std::optional<Token> Lexer::nextToken() {
    if (mHasError) {
        throw LexerException(
            "nextToken can no longer be called since "
            "the lexer is in panic mode");
    }

    if (isAtEnd(0)) {
        return Token(mLine, mColumn,
                     Token::Type::END_OF_FILE);
    }

    auto [state, lexeme] = simulateDFSA();

    return getTokenByFinalState(state, lexeme);
}

std::list<Error>& Lexer::getAllErrors() {
    if (!mHasError) {
        throw LexerException(
            "getAllErrors cannot be called if the "
            "lexer is not in panic mode");
    }

    while (!isAtEnd(0)) {
        auto [state, lexeme] = simulateDFSA();

        if (state == INVALID_STATE) {
            mErrorList.push_back(
                createError(lexeme, "unexpected lexeme"));
        }
    }

    return mErrorList;
}

DFSA Lexer::getDFSA() const {
    return mDFSA;
}

bool Lexer::hasError() const {
    return mHasError;
}

inline Token Lexer::createToken(Token::Type type,
                                std::string lexeme) const {
    return Token(mPrevLine, mPrevColumn, type, lexeme);
}

inline Token Lexer::createToken(Token::Type type) const {
    return Token(mPrevLine, mPrevColumn, type);
}

inline Error Lexer::createError(std::string lexeme,
                                std::string message) const {
    return Error(mPrevLine, mPrevColumn, lexeme, message);
}

bool Lexer::isAtEnd(size_t offset) const {
    return mCurrent + offset >= mSource.length();
}

std::optional<char> Lexer::nextCharacater(
    size_t cursor) const {
    if (!isAtEnd(cursor)) {
        return mSource[mCurrent + cursor];
    } else {
        return {};
    }
}

int Lexer::categoryOf(char character) const {
    for (auto& [category, check] : mCategoryToChecker) {
        if (check(character))
            return category;
    }

    return -1;  // no category
}

std::pair<int, std::string> Lexer::simulateDFSA() {
    size_t cursor = 0;
    int state = mDFSA.getInitialState();
    std::stack<int> stack;
    std::string lexeme;

    if (mFoundNewLine) {
        mFoundNewLine = false;

        mLine++;
        mColumn = 1;
    }

    mPrevLine = mLine;
    mPrevColumn = mColumn;

    for (;;) {
        if (mDFSA.isFinalState(state)) {
            while (!stack.empty()) {
                stack.pop();
            }
        }

        stack.push(state);

        std::optional<char> character =
            nextCharacater(cursor);

        if (!character.has_value())
            break;  // end of file

        state = mDFSA.getTransition(
            state, categoryOf(character.value()));

        if (state == INVALID_STATE)
            break;  // no more transitions are available

        // TODO: we have to be careful about this
        // especially if we use a buffering approach. We
        // will need to somehow, be able to back track
        // within our buffer. Or decouple the forward
        // scanning maybe?
        cursor++;

        // NOTE: this keeps track of the current line
        // and we place this here as it guarantees a
        // character consumption.
        if (character.value() == '\n')
            mFoundNewLine = true;

        lexeme += character.value();
    }

    size_t unconsumable_character_index = cursor;

    size_t next_start = cursor;

    // Suppose we accept two a's followed by any thing a
    // followed by a b and then followed by an a and
    // then anything. i.e. aa*b or aba*b aa*b == 1 -a->
    // 2 -a-> f3 s(b)
    //
    // aba*b == 1 -a-> 2 -b-> 3 -a-> f4 s(b)
    //
    // so for abb# we will get [1, 2, 3] in the stack,
    // next_start = 3 and lexeme = "ab".
    //
    // Then it will reach an invalid state.
    //
    // But non of our states are final therefore, we get
    // the following rollback:
    // 1. [1, 2], next_start = 2, lexeme = "a"
    // 2. [1], next_start = 1, lexeme = ""
    // 3. [], next_start = 0, break (since stack is empty)

    // NOTE: The stack is never empty we always start
    // with the initial state.

    for (;;) {
        state = stack.top();

        stack.pop();

        if (mDFSA.isFinalState(state)) {
            mCurrent += next_start;
            mColumn += next_start;

            return {state, lexeme};
        }

        next_start--;

        if (stack.empty())
            break;

        lexeme.pop_back();
    }

    std::pair<int, std::string> return_pair = {
        INVALID_STATE,
        mSource.substr(mCurrent,
                       unconsumable_character_index + 1)};

    // update current and column
    mCurrent += unconsumable_character_index + 1;
    mColumn += unconsumable_character_index + 1;

    return return_pair;
}

std::optional<Token> Lexer::getTokenByFinalState(
    int state, std::string lexeme) {
    if (mFinalStateToTokenType.find(state) ==
        mFinalStateToTokenType.end()) {
        mHasError = true;

        mErrorList.push_back(
            createError(lexeme, "unexpected lexeme"));

        return {};
    }

    return createToken(mFinalStateToTokenType[state],
                       lexeme);
}

}  // namespace Vought

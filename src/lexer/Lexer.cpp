// fmt
#include <fmt/core.h>
#include <pthread.h>

// vought
#include <lexer/DFSA.hpp>
#include <lexer/Lexer.hpp>

// std
#include <stack>

// macro definitions
#define NO_CATEGORY -1

namespace Vought {

char const* LexerException::what() const noexcept {
    return mMessage.c_str();
}

std::optional<Token> Lexer::nextToken() {
    if (mHasError)
        throw LexerException(
            "nextToken can no longer be called since "
            "the lexer is in panic mode");

    if (isAtEnd(0))
        return Token(mLine, mColumn,
                     Token::Type::END_OF_FILE);

    auto [state, lexeme] = simulateDFSA();

    if (state == INVALID_STATE) {
        mHasError = true;

        mErrorList.push_back(
            createError(lexeme, "unexpected lexeme"));

        return {};
    }

    Token::Type type = mFinalStateToTokenType[state];

    return createToken(lexeme, type);
}

std::list<Error>& Lexer::getAllErrors() {
    if (!mHasError) {
        throw LexerException(
            "getAllErrors cannot be called if the "
            "lexer is not in panic mode");
    }

    while (!isAtEnd(0)) {
        auto [state, lexeme] = simulateDFSA();

        if (state == INVALID_STATE)
            mErrorList.push_back(
                createError(lexeme, "unexpected lexeme"));
    }

    return mErrorList;
}

DFSA Lexer::getDFSA() const {
    return mDFSA;
}

bool Lexer::hasError() const {
    return mHasError;
}

inline Token Lexer::createToken(std::string lexeme,
                                Token::Type type) const {
    return Token(mPrevLine, mPrevColumn, lexeme, type);
}

inline Error Lexer::createError(std::string lexeme,
                                std::string message) const {
    return Error(mPrevLine, mPrevColumn, lexeme, message);
}

bool Lexer::isAtEnd(size_t offset) const {
    return mCursor + offset >= mSource.length();
}

std::optional<char> Lexer::nextCharacater(
    size_t offset) const {
    if (!isAtEnd(offset))
        return mSource[mCursor + offset];

    return {};
}

int Lexer::categoryOf(char character) const {
    for (auto& [category, check] : mCategoryToChecker) {
        if (check(character))
            return category;
    }

    return NO_CATEGORY;
}

std::pair<int, std::string> Lexer::simulateDFSA() {
    int state = mDFSA.getInitialState();

    size_t lCursor = 0;  // local cursor
    std::stack<int> stack;
    std::string lexeme;

    // if (mFoundNewLine) {
    //     mFoundNewLine = false;
    //
    //     mLine++;
    //     mColumn = 1;
    // }

    mPrevLine = mLine;
    mPrevColumn = mColumn;

    for (;;) {
        if (mDFSA.isFinalState(state)) {
            while (!stack.empty()) {
                stack.pop();
            }
        }

        stack.push(state);

        std::optional<char> ch = nextCharacater(lCursor);

        if (!ch.has_value())
            break;  // end of file

        int category = categoryOf(ch.value());

        if (category != NO_CATEGORY)
            state = mDFSA.getTransition(state, category);
        else
            state = INVALID_STATE;

        if (state == INVALID_STATE)
            break;  // no more transitions are available

        // TODO: we have to be careful about this
        // especially if we use a buffering approach. We
        // will need to somehow, be able to back track
        // within our buffer. Or decouple the forward
        // scanning maybe?
        lCursor++;

        // NOTE: this keeps track of the current line
        // and we place this here as it guarantees a
        // character consumption.
        if (ch.value() == '\n') {
            // mFoundNewLine = true;

            mLine++;
            mColumn = 1;
        }

        lexeme += ch.value();
    }

    size_t unconsumable_ch_index = lCursor;

    size_t next_start = lCursor;

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
            mCursor += next_start;
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
        mSource.substr(mCursor, unconsumable_ch_index + 1)};

    // update cursor and column
    mCursor += unconsumable_ch_index + 1;
    mColumn += unconsumable_ch_index + 1;

    return return_pair;
}

LexerBuilder& LexerBuilder::addSource(
    std::string const& source) {
    mSource = source;

    return *this;
}

LexerBuilder& LexerBuilder::addCategory(
    int category, std::function<bool(char)> checker) {
    if (category < 0)
        throw LexerException(fmt::format(
            "tried to initialise with negative category {}",
            category));

    mCategories[category] = checker;

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

LexerBuilder& LexerBuilder::addTransition(
    int state, std::initializer_list<int> categories,
    int nextState) {
    for (int category : categories)
        addTransition(state, category, nextState);

    return *this;
}

LexerBuilder& LexerBuilder::addComplementaryTransition(
    int state, std::initializer_list<int> categories,
    int nextState) {
    for (int category : categories) {
        if (mCategories.count(category) <= 0)
            throw LexerException(fmt::format(
                "tried to add a transition using "
                "an unregistered category {}",
                category));
    }

    for (auto& [category, _] : mCategories) {
        if (std::find(categories.begin(), categories.end(),
                      category) == categories.end())
            addTransition(state, category, nextState);
    }

    return *this;
}

LexerBuilder& LexerBuilder::addComplementaryTransition(
    int state, int category, int nextState) {
    return addComplementaryTransition(state, {category},
                                      nextState);
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
        int state = input.x;
        int category = input.y;

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

}  // namespace Vought

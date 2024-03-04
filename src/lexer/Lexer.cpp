// fmt
#include <fmt/core.h>

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

    std::optional<Token> token{};

    if (state == INVALID_STATE) {
        mHasError = true;

        mErrorList.push_back(
            createError(lexeme, "unexpected lexeme"));
    } else {
        token = createToken(lexeme,
                            mFinalStateToTokenType[state]);
    }

    updateLocationState(lexeme);

    return token;
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

        updateLocationState(lexeme);
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
    return Token(mLine, mColumn, lexeme, type);
}

inline Error Lexer::createError(std::string lexeme,
                                std::string message) const {
    return Error(mLine, mColumn, lexeme, message);
}

bool Lexer::isAtEnd(size_t offset) const {
    return mCursor + offset >= mSource.length();
}

void Lexer::updateLocationState(std::string& lexeme) {
    for (char ch : lexeme) {
        mCursor++;

        if (ch == '\n') {
            mLine++;

            mColumn = 1;
        } else {
            mColumn++;
        }
    }
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

        lexeme += ch.value();
    }

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

        if (mDFSA.isFinalState(state))
            return {state, lexeme};

        if (stack.empty())
            break;

        lexeme.pop_back();
    }

    return {INVALID_STATE,
            mSource.substr(mCursor, lCursor + 1)};
}

}  // namespace Vought

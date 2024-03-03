#include <fmt/core.h>

#include <lexer/Lexer.hpp>
#include <stack>

namespace Vought {

char const* Lexer::LexerException::what() const noexcept {
    return mMessage.c_str();
}

Lexer& Lexer::createTransition(int state, int letter,
                               int resultantState) {
    mDFSA.setTransition(state, letter, resultantState);

    return *this;
}

Lexer& Lexer::createTransitionAsFinal(int state, int letter,
                                      int resultantState,
                                      Token::Type type) {
    mDFSA.setTransitionAsFinal(state, letter,
                               resultantState);

    mStateAssociation[resultantState] = type;

    return *this;
}

std::optional<Token> Lexer::nextToken() {
    if (mHasError) {
        throw LexerException(
            "nextToken can no longer be called since "
            "the lexer is in panic mode");
    }

    if (isAtEnd()) {
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

    while (!isAtEnd()) {
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

bool Lexer::isAtEnd() const {
    return mCurrent >= mSource.length();
}

bool Lexer::isAtEnd(size_t cursor) const {
    return mCurrent + cursor >= mSource.length();
}

std::optional<char> Lexer::nextCharacater(
    size_t cursor) const {
    if (!isAtEnd(cursor)) {
        return mSource[mCurrent + cursor];
    } else {
        return {};
    }
}

Category Lexer::categoryOf(char character) const {
    switch (character) {
        case '&':
            return AND;
        case '!':
            return BANG;
        case ':':
            return COLON;
        case ',':
            return COMMA;
        case '.':
            return DOT;
        case '=':
            return EQUAL;
        case '>':
            return GREATER_THAN;
        case '{':
            return LEFT_BRACE;
        case '(':
            return LEFT_PAREN;
        case '<':
            return LESS_THAN;
        case '-':
            return MINUS;
        case '|':
            return PIPE;
        case '+':
            return PLUS;
        case '}':
            return RIGHT_BRACE;
        case ')':
            return RIGHT_PAREN;
        case ';':
            return SEMICOLON;
        case '/':
            return SLASH;
        case '*':
            return STAR;
        case '_':
            return UNDERSCORE;
        default:
            if (isalpha(character))
                return LETTER;

            if (isdigit(character))
                return DIGIT;

            if (isspace(character))
                return WHITESPACE;
    }

    return OTHER;
}

std::pair<int, std::string> Lexer::simulateDFSA() {
    size_t cursor = 0;
    int state = START_STATE;
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
        if (mDFSA.isAcceptingState(state)) {
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

        if (mDFSA.isAcceptingState(state)) {
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
    if (mStateAssociation.find(state) ==
        mStateAssociation.end()) {
        mHasError = true;

        mErrorList.push_back(
            createError(lexeme, "unexpected lexeme"));

        return {};
    }

    return createToken(mStateAssociation[state], lexeme);
}

}  // namespace Vought

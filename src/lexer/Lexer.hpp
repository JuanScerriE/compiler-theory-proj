#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <common/Item.hpp>
#include <exception>
#include <iostream>
#include <list>
#include <optional>
#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace Vought {

enum Category {
    AND,
    BANG,
    COLON,
    COMMA,
    DIGIT,
    DOT,
    EQUAL,
    GREATER_THAN,
    LEFT_BRACE,
    LEFT_PAREN,
    LESS_THAN,
    LETTER,
    MINUS,
    PIPE,
    PLUS,
    RIGHT_BRACE,
    RIGHT_PAREN,
    SEMICOLON,
    SLASH,
    STAR,
    UNDERSCORE,
    WHITESPACE,

    OTHER,
    CATEGORY_SIZE
};

constexpr size_t intStringLen(int integer) {
    size_t length = 1;

    while (0 != (integer /= 10))
        length++;

    return length;
}

static std::string leftPad(std::string const& str,
                           size_t length,
                           char paddingCharacter) {
    if (str.length() >= length) {
        return str;
    } else {
        return std::string(length - str.length(),
                           paddingCharacter) +
               str;
    }
}

// NOTE: by default the -1 states is considered to be the
// error state and it is reached when no other possible
// transitions are available.

#define INVALID_STATE -1
#define START_STATE 0

class DFSA {
   public:
    class DFSAException : public std::exception {
       public:
        DFSAException(char const* message)
            : mMessage(message) {
        }
        DFSAException(std::string message)
            : mMessage(message) {
        }

        [[nodiscard]] char const* what()
            const noexcept override {
            return mMessage.c_str();
        }

       private:
        std::string mMessage;
    };

    explicit DFSA(int noOfStates, int noOfLetters)
        : mNoOfStates(noOfStates),
          mNoOfLetters(noOfLetters),
          mTransitionTable(std::vector<std::vector<int>>(
              noOfStates,
              std::vector<int>(noOfLetters,
                               INVALID_STATE))) {
    }

    bool isValidState(int state) const {
        return START_STATE <= state && state < mNoOfStates;
    }

    bool isValidLetter(int letter) const {
        return START_STATE <= letter &&
               letter < mNoOfLetters;
    }

    bool isAcceptingState(int state) const {
        return std::find(mAcceptingStates.begin(),
                         mAcceptingStates.end(), state) !=
               std::end(mAcceptingStates);
    }

    void setTransition(int currentState, int letter,
                       int resultantState) {
        if (!isValidState(currentState))
            throw DFSAException("state does not exist");

        if (!isValidState(resultantState))
            throw DFSAException("state does not exist");

        if (!isValidLetter(letter))
            throw DFSAException("letter does not exist");

        mTransitionTable[currentState][letter] =
            resultantState;
    }

    void setTransition(int currentState, int letter,
                       int resultantState, bool isFinal) {
        if (isFinal) {
            mAcceptingStates.insert(resultantState);
        }

        setTransition(currentState, letter, resultantState);
    }

    int getTransition(int currentState, int letter) const {
        if (!isValidState(currentState))
            throw DFSAException("state does not exist");

        if (!isValidLetter(letter))
            throw DFSAException("letter does not exist");

        return mTransitionTable[currentState][letter];
    }

    friend std::ostream& operator<<(std::ostream& out,
                                    DFSA const& dfsa) {
        // we use +1 to handle the fact that we will most
        // likely have a - in front and that's an extra
        // character.
        size_t length = intStringLen(dfsa.mNoOfStates) + 1;

        out << "Accepting States: ";

        for (int state : dfsa.mAcceptingStates) {
            out << state << ", ";
        }

        out << "\n";

        for (int i = 0; i < dfsa.mNoOfStates; i++) {
            for (int j = 0; j < dfsa.mNoOfLetters; j++) {
                out << leftPad(
                    std::to_string(
                        dfsa.mTransitionTable[i][j]),
                    length, ' ');

                if (j < dfsa.mNoOfLetters - 1)
                    out << ", ";
            }

            out << "\n";
        }

        return out;
    }

   private:
    int mNoOfStates;
    int mNoOfLetters;
    std::set<int> mAcceptingStates;
    std::vector<std::vector<int>> mTransitionTable;
};

class Lexer {
   public:
    class LexerException : public std::exception {
       public:
        LexerException(char const* message)
            : mMessage(message) {
        }
        LexerException(std::string message)
            : mMessage(message) {
        }

        [[nodiscard]] char const* what()
            const noexcept override {
            return mMessage.c_str();
        }

       private:
        std::string mMessage;
    };

    explicit Lexer(std::string const& source)
        : mSource(source), mDFSA(DFSA(30, CATEGORY_SIZE)) {
        // whitespace
        mDFSA.setTransition(START_STATE, WHITESPACE, 1,
                            true);
        mDFSA.setTransition(1, WHITESPACE, 1);

        // identifier
        mDFSA.setTransition(START_STATE, LETTER, 2, true);
        mDFSA.setTransition(START_STATE, UNDERSCORE, 2);
        mDFSA.setTransition(2, LETTER, 2);
        mDFSA.setTransition(2, DIGIT, 2);
        mDFSA.setTransition(2, UNDERSCORE, 2);

        // integers & floats
        mDFSA.setTransition(START_STATE, DIGIT, 3, true);
        mDFSA.setTransition(3, DIGIT, 3);
        mDFSA.setTransition(3, DOT, 4, true);
        mDFSA.setTransition(4, DIGIT, 4);

        // puncutation "(", ")", "{", "}", ";", ",", ":"
        mDFSA.setTransition(START_STATE, LEFT_PAREN, 5,
                            true);
        mDFSA.setTransition(START_STATE, RIGHT_PAREN, 6,
                            true);
        mDFSA.setTransition(START_STATE, LEFT_BRACE, 7,
                            true);
        mDFSA.setTransition(START_STATE, RIGHT_BRACE, 8,
                            true);
        mDFSA.setTransition(START_STATE, SEMICOLON, 9,
                            true);
        mDFSA.setTransition(START_STATE, COMMA, 10, true);
        mDFSA.setTransition(START_STATE, COLON, 11, true);

        // "=", "=="
        mDFSA.setTransition(START_STATE, EQUAL, 12, true);
        mDFSA.setTransition(12, EQUAL, 13, true);

        // "<", "<="
        mDFSA.setTransition(START_STATE, LESS_THAN, 14,
                            true);
        mDFSA.setTransition(14, EQUAL, 15, true);

        // ">", ">="
        mDFSA.setTransition(START_STATE, GREATER_THAN, 16,
                            true);
        mDFSA.setTransition(16, EQUAL, 17, true);

        // "-", "->"
        mDFSA.setTransition(START_STATE, MINUS, 18, true);
        mDFSA.setTransition(18, GREATER_THAN, 19, true);

        // "*", "**"
        mDFSA.setTransition(START_STATE, STAR, 20, true);
        mDFSA.setTransition(20, STAR, 21, true);

        // "!", "!="
        mDFSA.setTransition(START_STATE, BANG, 22, true);
        mDFSA.setTransition(22, EQUAL, 23, true);

        // "&&", "||"
        mDFSA.setTransition(START_STATE, AND, 24);
        mDFSA.setTransition(24, AND, 25, true);
        mDFSA.setTransition(START_STATE, PIPE, 26);
        mDFSA.setTransition(26, PIPE, 27, true);

        // "+"
        mDFSA.setTransition(START_STATE, PLUS, 28, true);

        // division
        mDFSA.setTransition(START_STATE, SLASH, 29, true);
    }

    std::optional<Token> nextToken() {
        if (mHasError) {
            throw LexerException(
                "nextToken can no longer be called since "
                "the lexer is in panic mode");
        }

        if (isAtEnd()) {
            return createToken(Token::Type::END_OF_FILE);
        }

        auto [state, lexeme] = simulateDFSA();

        mCurrent += lexeme.size();

        return getTokenByFinalState(state, lexeme);
    }

    std::list<Error>& getAllErrors() {
        if (!mHasError) {
            throw LexerException(
                "getAllErrors cannot be called if the "
                "lexer is not in panic mode");
        }

        while (!isAtEnd()) {
            auto [state, lexeme] = simulateDFSA();

            mCurrent += lexeme.size();

            if (state == INVALID_STATE) {
                mErrorList.emplace_back(createError(
                    lexeme, "unexpected lexeme"));
            }
        }

        return mErrorList;
    }

    DFSA getDFSA() const {
        return mDFSA;
    }

    bool hasError() const {
        return mHasError;
    }

   private:
    // start of helper functions
    inline Token createToken(Token::Type type,
                             Value value) const {
        return Token(mLine, mCurrent, type, value);
    }

    inline Token createToken(Token::Type type) const {
        return Token(mLine, mCurrent, type);
    }

    inline Error createError(std::string lexeme,
                             std::string message) const {
        return Error(mLine, mCurrent, lexeme, message);
    }
    // end of helper functions

    bool isAtEnd(size_t cursor) const {
        return mCurrent + cursor >= mSource.length();
    }

    bool isAtEnd() const {
        return mCurrent >= mSource.length();
    }

    std::optional<char> nextCharacater(
        size_t cursor) const {
        if (!isAtEnd(cursor)) {
            return mSource[mCurrent + cursor];
        } else {
            return {};
        }
    }

    Category categoryOf(char character) const {
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

    std::optional<Token> getTokenByFinalState(
        int state, std::string lexeme) {
        switch (state) {
            case 1:
                return createToken(Token::Type::WHITESPACE);
            case 2:
                return createToken(
                    Token::Type::IDENTIFIER,
                    Value::createIdentifier(lexeme));
            case 3:
                return createToken(
                    Token::Type::INTEGER,
                    Value::createInteger(lexeme));
            case 4:
                return createToken(
                    Token::Type::FLOAT,
                    Value::createFloat(lexeme));
            case 5:
                return createToken(Token::Type::LEFT_PAREN);
            case 6:
                return createToken(
                    Token::Type::RIGHT_PAREN);
            case 7:
                return createToken(Token::Type::LEFT_BRACE);
            case 8:
                return createToken(
                    Token::Type::RIGHT_BRACE);
            case 9:
                return createToken(Token::Type::SEMICOLON);
            case 10:
                return createToken(Token::Type::COMMA);
            case 11:
                return createToken(Token::Type::COLON);
            case 12:
                return createToken(Token::Type::EQUAL);
            case 13:
                return createToken(
                    Token::Type::EQUAL_EQUAL);
            case 14:
                return createToken(Token::Type::LESS);
            case 15:
                return createToken(Token::Type::LESS_EQUAL);
            case 16:
                return createToken(Token::Type::GREATER);
            case 17:
                return createToken(
                    Token::Type::GREATER_EQUAL);
            case 18:
                return createToken(Token::Type::MINUS);
            case 19:
                return createToken(
                    Token::Type::RETURN_TYPE);
            case 20:
                return createToken(Token::Type::STAR);
            case 21:
                return createToken(Token::Type::EXPONENT);
            case 22:
                return createToken(Token::Type::BANG);
            case 23:
                return createToken(Token::Type::BANG_EQUAL);
            case 25:
                return createToken(Token::Type::AND);
            case 27:
                return createToken(Token::Type::OR);
            case 28:
                return createToken(Token::Type::PLUS);
            case 29:
                return createToken(Token::Type::SLASH);
            default:
                mHasError = true;

                mErrorList.emplace_back(createError(
                    lexeme, "unexpected lexeme"));

                return {};
        }
    }

    std::pair<int, std::string> simulateDFSA() {
        size_t cursor = 0;
        int state = START_STATE;
        std::stack<int> stack;
        std::string lexeme;

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
                break;

            state = mDFSA.getTransition(
                state, categoryOf(character.value()));

            // TODO: we have to be careful about this
            // especially if we use a buffering approach. We
            // will need to somehow, be able to back track
            // within our buffer. Or decouple the forward
            // scanning maybe?
            cursor++;

            if (state == INVALID_STATE)
                break;

            // NOTE: this keeps track of the current line
            // and we place this here as it guarantees a
            // character consumption.
            if (character.value() == '\n')
                mLine++;

            lexeme += character.value();
        }

        while (!stack.empty()) {
            state = stack.top();

            stack.pop();

            if (mDFSA.isAcceptingState(state)) {
                return {state, lexeme};
            }

            if (!lexeme.empty())
                lexeme.pop_back();
        }

        return {INVALID_STATE,
                mSource.substr(mCurrent, cursor)};
    }

    // source info
    size_t mCurrent = 0;
    int mLine = 0;
    std::string mSource;

    // dfsa
    DFSA mDFSA;

    // error info
    bool mHasError = false;
    std::list<Error>
        mErrorList;  // TODO: make it a generator(oof
                     // coroutines)?
};

}  // namespace Vought

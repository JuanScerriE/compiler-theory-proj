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
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace Vought {

enum Category {
    LETTER,
    DIGIT,
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

    explicit DFSA(int noOfStates, int noOfLetters,
                  std::vector<int> acceptingStates)
        : mNoOfStates(noOfStates),
          mNoOfLetters(noOfLetters),
          mTransitionTable(std::vector<std::vector<int>>(
              noOfStates,
              std::vector<int>(noOfLetters,
                               INVALID_STATE))) {
        for (int state : acceptingStates) {
            if (!isValidState(state)) {
                throw DFSAException("state does not exist");
            }
        }

        mAcceptingStates = acceptingStates;
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

        for (int i = 0; i < dfsa.mAcceptingStates.size();
             i++) {
            out << dfsa.mAcceptingStates[i];

            if (i < dfsa.mAcceptingStates.size() - 1)
                out << ", ";
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
    std::vector<int> mAcceptingStates;
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
        : mSource(source),
          mDFSA(DFSA(3, CATEGORY_SIZE, {1, 2})) {
        // identifier
        mDFSA.setTransition(START_STATE, LETTER, 1);
        mDFSA.setTransition(START_STATE, UNDERSCORE, 1);
        mDFSA.setTransition(1, LETTER, 1);
        mDFSA.setTransition(1, DIGIT, 1);
        mDFSA.setTransition(1, UNDERSCORE, 1);

        // whitespace
        mDFSA.setTransition(START_STATE, WHITESPACE, 2);
        mDFSA.setTransition(2, WHITESPACE, 2);
    }

    std::optional<Token> nextToken() {
        if (mHasError) {
            throw LexerException(
                "nextToken can no longer be called since "
                "the lexer is in panic mode");
        }

        if (isAtEnd()) {
            return Token("", mLine,
                         Token::Type::END_OF_FILE,
                         Value::createNil());
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
                mErrorList.emplace_back(Error(
                    lexeme, mLine, "unexpected lexeme"));
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
    std::optional<Token> getTokenByFinalState(
        int state, std::string lexeme) {
        switch (state) {
            case 1:
                return Token(lexeme, mLine,
                             Token::Type::IDENTIFIER,
                             Value::createNil());
            case 2:
                return Token(lexeme, mLine,
                             Token::Type::WHITESPACE,
                             Value::createNil());
            default:
                mHasError = true;

                mErrorList.emplace_back(Error(
                    lexeme, mLine, "unexpected lexeme"));

                return {};
        }
    }

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
        if (isalpha(character)) {
            return LETTER;
        }

        if (isdigit(character)) {
            return DIGIT;
        }

        if (character == '_') {
            return UNDERSCORE;
        }

        if (isspace(character)) {
            return WHITESPACE;
        }

        return OTHER;
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

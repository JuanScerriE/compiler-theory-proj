#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <common/Item.hpp>
#include <exception>
#include <iostream>
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

std::string leftPad(std::string const& str, size_t length,
                    char paddingCharacter) {
    if (str.length() >= length) {
        return str;
    } else {
        return std::string(length - str.length(),
                           paddingCharacter) +
               str;
    }
}

// transition_table[0][LETTER] = 1;
// transition_table[0][UNDERSCORE] = 1;
// transition_table[0][WHITESPACE] = 2;
// transition_table[1][LETTER] = 1;
// transition_table[1][DIGIT] = 1;
// transition_table[2][WHITESPACE] = 2;

// NOTE: by default the -1 states is considered to be the
// error state and it is reached when no other possible
// transitions are avaliable.

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
        // likely have a - infront and that's an extra
        // character.
        size_t length = intStringLen(dfsa.mNoOfStates) + 1;

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
    explicit Lexer(std::string source)
        : mSource(source), mDFSA(DFSA(3, 6, {1, 2})) {
    }

   private:
    std::variant<Token, Error> getTokenByFinalState(
        std::string lexeme) {
        switch (mState) {
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

                return Error(lexeme, mLine,
                             "unexpected lexeme");
        }
    }

    bool isAtEnd() const {
        return mCurrent >= mSource.length();
    }

    std::optional<char> nextCharacater() const {
        if (!isAtEnd()) {
            return mSource[mCurrent];
        } else {
            return std::optional<char>();
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
        int state = START_STATE;

        std::stack<int> stack;

        std::string lexeme;

        do {
            if (mDFSA.isAcceptingState(state)) {
                while (!stack.empty()) {
                    stack.pop();
                }
            }

            stack.push(state);

            std::optional<char> character =
                nextCharacater();

            if (!character.has_value()) {
                break;
            }

            if (character.value() == '\n') {
                mLine++;
            }

            lexeme += character.value();

            mCurrent++;

            state = mDFSA.getTransition(
                state, categoryOf(character.value()));
        } while (state != INVALID_STATE);

        std::string original_lexeme = lexeme;

        while (!stack.empty()) {
            state = stack.top();

            if (mDFSA.isAcceptingState(state)) {
                return std::make_pair(state, lexeme);
            }

            lexeme.pop_back();

            mCurrent--;

            stack.pop();
        }

        return {INVALID_STATE, original_lexeme};
    }

    // source info
    size_t mCurrent = 0;
    int mLine = 0;
    std::string mSource;

    // dfsa info
    // int mState = 0;
    // char mCharacter;
    DFSA mDFSA;

    // has error
    bool mHasError = false;
};

}  // namespace Vought

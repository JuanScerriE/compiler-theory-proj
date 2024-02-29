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
#include <stdexcept>
#include <string>
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

    explicit DFSA(std::vector<int> Q, std::vector<int> S,
                  std::vector<int> F)
        : mQ(Q),
          mS(S),
          mD(TransitionTable(Q.size(), S.size())) {
        for (int state : F) {
            if (!isValidState(state)) {
                throw DFSAException("state does not exist");
            }
        }
    }

    bool isValidState(int state) const {
        return std::find(mQ.begin(), mQ.end(), state) !=
               std::end(mQ);
    }

    bool isAcceptingState(int state) const {
        return std::find(mF.begin(), mF.end(), state) !=
               std::end(mF);
    }

    bool isValidLetter(int letter) const {
        return std::find(mS.begin(), mS.end(), letter) !=
               std::end(mS);
    }

    void setTransition(int currentState, int letter,
                       int resultantState) {
        if (!isValidState(currentState))
            throw DFSAException("state does not exist");

        if (!isValidState(resultantState))
            throw DFSAException("state does not exist");

        if (!isValidLetter(letter))
            throw DFSAException("letter does not exist");

        mD.setTransition(currentState, letter,
                         resultantState);
    }

    int getTransition(int currentState, int letter) const {
        return mD.getTransition(currentState, letter);
    }

   private:
    class TransitionTable {
       public:
        explicit TransitionTable(size_t QSize, size_t SSize)
            : mQSize(QSize), mSSize(SSize) {
            mTable = std::vector<std::vector<int>>(
                QSize, std::vector<int>(SSize, -1));
        }

        void setTransition(int currentState, int letter,
                           int resultantState) {
            mTable[currentState][letter] = resultantState;
        }

        int getTransition(int currentState,
                          int letter) const {
            return mTable[currentState][letter];
        }

        friend std::ostream& operator<<(
            std::ostream& out,
            TransitionTable const& table) {
            size_t length = 0;

            for (int i = 0; i < table.mQSize; i++) {
                for (int j = 0; j < table.mSSize; j++) {
                    length = std::max(
                        length,
                        intStringLen(table.mTable[i][j]) +
                            1);
                }
            }

            for (int i = 0; i < table.mQSize; i++) {
                for (int j = 0; j < table.mSSize; j++) {
                    out << leftPad(
                        std::to_string(table.mTable[i][j]),
                        length, ' ');

                    if (j < table.mSSize - 1)
                        out << ", ";
                }

                out << "\n";
            }

            return out;
        }

        size_t getSizeOfQ() const {
            return mQSize;
        }

        size_t getSizeOfS() const {
            return mSSize;
        }

       private:
        size_t mQSize = 0;
        size_t mSSize = 0;

        std::vector<std::vector<int>> mTable;
    };

    std::vector<int> mQ;  // states
    std::vector<int> mS;  // sigma
    std::vector<int> mF;  // final states

    TransitionTable mD;  // delta
};

class Lexer {
   public:
    explicit Lexer(std::string source)
        : mSource(source),
          mDFSA(DFSA({0, 1, 2, 3}, {0, 1, 2, 3}, {1, 2})) {
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

    std::optional<int> simulateDFSA() {
        int state = 0;
        std::stack<int> stack;
        std::string lexeme;
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

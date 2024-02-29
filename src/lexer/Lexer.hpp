#pragma once

#include <array>
#include <common/Token.hpp>
#include <iostream>
#include <ostream>
#include <string>

namespace Vought {

enum Alphabet {
    UNDERSCORE,
    LETTER,
    DIGIT,
    WHITESPACE,
    OTHER,
    ALPHABET_SIZE
};

constexpr int intlen(int integer) {
    int len = 1;

    while (0 != (integer /= 10))
        len++;

    return len;
}

template <size_t STATES>
class TransitionTable {
   public:
    constexpr explicit TransitionTable() {
        for (size_t i = 0; i < STATES; i++) {
            for (size_t j = 0; j < ALPHABET_SIZE; j++) {
                transition_table[i][j] = -1;
            }
        }

        transition_table[0][LETTER] = 1;
        transition_table[0][UNDERSCORE] = 1;
        transition_table[0][WHITESPACE] = 2;
        transition_table[1][LETTER] = 1;
        transition_table[1][DIGIT] = 1;
        transition_table[2][WHITESPACE] = 2;
    }

    // TODO: make these use the [] operator
    // void set(size_t i, size_t j, int transition) {
    //     transition_table[i][j] = transition;
    // }
    // int transition(size_t i, size_t j) {
    //     return transition_table[i][j];
    // }

    // TODO: make this use operator<<
    void print() {
        constexpr int size = intlen(STATES - 1) + 1;

        char state[size + 1];

        for (int i = 0; i < STATES; i++) {
            for (int j = 0; j < ALPHABET_SIZE; j++) {
                printf("%*d", size, transition_table[i][j]);

                if (j < ALPHABET_SIZE - 1)
                    std::cout << ", ";
            }

            std::cout << "\n";
        }
    }

   private:
    std::array<std::array<int, ALPHABET_SIZE>, STATES>
        transition_table;
};

class Lexer {
   public:
    explicit Lexer(std::string source);

   private:
    bool is_accepting(int state) {
        return std::find(accepting_states.begin(),
                         accepting_states.end(), state) !=
               std::end(accepting_states);
    }

    Token get_token_by_final_state(int state,
                                   std::string lexeme) {
        switch (state) {
            case 1:
                return Token(Token::Type::IDENTIFIER, lexeme, Value::createNil(), line);
            case 2:
                return Token(Token::Type::WHITESPACE, lexeme, Value::createNil(), line);
            default:
                return Token(Token::Type::ERROR, lexeme, Value::createError(), line);
        }
    }

    std::vector<int> accepting_states = {1, 2};
    TransitionTable<3> transition_table;
    int line = 0;
    size_t current = 0;
    size_t start = 0;
};

}  // namespace Vought

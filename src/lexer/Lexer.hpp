#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <ostream>
#include <string>

namespace Vought {

enum TokenType {
};

enum Alphabet {
    UNDERSCORE,
    LETTER,
    DIGIT,
    WHITESPACE,
    OTHER,
    ALPHABET_SIZE
};

template <size_t STATES, size_t ALPHABET>
class TransitionTable {
   public:
    constexpr explicit TransitionTable() {
        for (size_t i = 0; i < STATES; i++) {
            for (size_t j = 0; j < ALPHABET; j++) {
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
    void set(size_t i, size_t j, int transition) {
        transition_table[i][j] = transition;
    }
    int transition(size_t i, size_t j) {
        return transition_table[i][j];
    }

    std::ostream& operator<<(std::ostream& out) {
        constexpr size_t size =
            static_cast<size_t>(floor(log10(STATES - 1))) +
            2;

        char state[size];

        for (size_t i = 0; i < STATES; i++) {
            for (size_t j = 0; j < ALPHABET; j++) {
                snprintf(state, size, "%d",
                         transition_table[i][j]);

                if (j < ALPHABET - 1)
                    out << ", ";
            }

            if (i < STATES - 1)
                out << "\n";
        }
    }

   private:
    std::array<std::array<int, ALPHABET>, STATES>
        transition_table;
};

class Lexer {
   public:
    explicit Lexer(std::string source);

   private:
    TransitionTable<3, ALPHABET_SIZE> transition_table;
    int line = 0;
    size_t current = 0;
    size_t start = 0;
};

}  // namespace Vought

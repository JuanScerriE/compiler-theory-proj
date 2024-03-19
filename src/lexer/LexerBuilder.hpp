#pragma once

// vought
#include <common/Error.hpp>
#include <common/Token.hpp>
#include <lexer/Lexer.hpp>

// std
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Vought {

struct Pair {
    int x;
    int y;

    bool operator==(Pair const& other) const {
        return (x == other.x) && (y == other.y);
    }

    // NOTE: the below has function requires both x and y to
    // be positive which is our case they are
    struct Hash {
        // http://szudzik.com/ElegantPairing.pdf
        size_t operator()(Pair const& pair) const {
            if (pair.x != std::max(pair.x, pair.y))
                return pair.y * pair.y + pair.x;
            else
                return pair.x * pair.x + pair.x + pair.y;
        }
    };
};

class LexerBuilder {
   public:
    LexerBuilder& addSource(std::string const& source);

    LexerBuilder& addCategory(
        int category, std::function<bool(char)> checker);

    LexerBuilder& setInitialState(int state);

    LexerBuilder& addTransition(int state, int category,
                                int resultantState);

    LexerBuilder& addTransition(
        int state, std::initializer_list<int> categories,
        int resultantState);

    LexerBuilder& addComplementaryTransition(
        int state, int category, int resultantState);

    LexerBuilder& addComplementaryTransition(
        int state, std::initializer_list<int> categories,
        int resultantState);

    LexerBuilder& setStateAsFinal(int state,
                                  Token::Type type);

    LexerBuilder& reset();

    Lexer build();

   private:
    std::string mSource{};  // initialise

    std::unordered_set<int> mStates{};

    std::unordered_map<int, std::function<bool(char)>>
        mCategories{};

    std::unordered_map<Pair, int, Pair::Hash>
        mTransitions{};

    std::optional<int> mInitialState{};  // initialise

    std::unordered_map<int, Token::Type> mFinalStates{};
};

}  // namespace Vought

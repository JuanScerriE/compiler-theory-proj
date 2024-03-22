#pragma once

// std
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

// vought
#include <common/Error.hpp>
#include <common/Token.hpp>
#include <lexer/Dfsa.hpp>

namespace PArL {

class Lexer {
   public:
    Lexer(Dfsa dfsa,
          std::unordered_map<int, std::function<bool(char)>>
              categoryToChecker,
          std::unordered_map<int, Token::Type>
              finalStateToTokenType);

    std::optional<Token> nextToken();

    [[nodiscard]] Dfsa const& getDfsa() const;

    bool hasError() const;
    void reset();
    void addSource(std::string const& source);

   private:
    [[nodiscard]] Token createToken(std::string const& lexeme,
                      Token::Type type) const;
    //Error createError(std::string const& lexeme) const;
    void printError(Error const& error);
    void findRemainingErrors();

    bool isAtEnd(size_t offset) const;

    [[nodiscard]] std::optional<char> nextCharacter(size_t cursor) const;
    [[nodiscard]] std::vector<int> categoriesOf(char character) const;
    [[nodiscard]] std::pair<int, std::string> simulateDFSA();

    void updateLocationState(std::string const& lexeme);

    // source info
    size_t mCursor = 0;

    int mLine = 1;
    int mColumn = 1;

    std::string mSource{};

    // error info
    bool mHasError = false;

    // dfsa
    const Dfsa mDfsa;

    // category checkers
    const std::unordered_map<int, std::function<bool(char)>>
        mCategoryToChecker;

    // final state to token type association
    const std::unordered_map<int, Token::Type>
        mFinalStateToTokenType;
};

}  // namespace PArL

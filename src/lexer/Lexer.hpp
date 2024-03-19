#pragma once

// vought
#include <common/Error.hpp>
#include <common/Token.hpp>
#include <lexer/DFSA.hpp>

// std
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace Vought {

class Lexer {
   public:
    Lexer(DFSA const& dfsa,
          std::unordered_map<int, std::function<bool(char)>> const&
              categoryToChecker,
          std::unordered_map<int, Token::Type> const&
              finalStateToTokenType);

    std::optional<Token> nextToken();
    DFSA getDFSA() const;
    bool hasError() const;
    void reset();
    void addSource(std::string const& source);

   private:
    Token createToken(std::string const& lexeme,
                      Token::Type type) const;
    Error createError(std::string const& lexeme) const;
    void printError(Error const& error);
    void findRemainingErrors();
    bool isAtEnd(size_t offset) const;
    void updateLocationState(std::string const& lexeme);
    std::optional<char> nextCharacater(size_t cursor) const;
    std::vector<int> categoriesOf(char character) const;
    std::pair<int, std::string> simulateDFSA();

    // source info
    size_t mCursor = 0;

    int mLine = 1;
    int mColumn = 1;

    std::string mSource{};

    // error info
    bool mHasError = false;

    // dfsa
    const DFSA mDFSA;

    // category checkers
    const std::unordered_map<int, std::function<bool(char)>>
        mCategoryToChecker;

    // final state to token type association
    const std::unordered_map<int, Token::Type>
        mFinalStateToTokenType;
};

}  // namespace Vought

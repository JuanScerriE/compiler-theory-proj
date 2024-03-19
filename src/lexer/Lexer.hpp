#pragma once

// vought
#include <common/Error.hpp>
#include <common/Token.hpp>
#include <lexer/DFSA.hpp>

// std
#include <cstddef>
#include <functional>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace Vought {

class Lexer {
   public:
    std::optional<Token> nextToken();

    DFSA getDFSA() const;

    bool hasError() const;

    std::list<Error>& getAllErrors();

   private:
    Token createToken(std::string lexeme,
                      Token::Type type) const;

    Error createError(std::string messgae,
                      std::string lexeme) const;

    bool isAtEnd(size_t offset) const;

    void updateLocationState(std::string& lexeme);

    std::optional<char> nextCharacater(size_t cursor) const;

    std::vector<int> categoriesOf(char character) const;

    std::pair<int, std::string> simulateDFSA();

    friend class LexerBuilder;

    // source info
    size_t mCursor = 0;

    int mLine = 1;
    int mColumn = 1;

    std::string mSource;

    // dfsa
    DFSA mDFSA;

    // category checkers
    std::unordered_map<int, std::function<bool(char)>>
        mCategoryToChecker;

    // final state to token type association
    std::unordered_map<int, Token::Type>
        mFinalStateToTokenType;

    // error info
    bool mHasError = false;

    std::list<Error>
        mErrorList;  // TODO: make it a generator(oof
                     // coroutines)?
};

}  // namespace Vought

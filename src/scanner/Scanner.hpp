#pragma once

// std
#include <unordered_map>
#include <vector>

// lox
#include <common/Token.hpp>

namespace Lox {

class Scanner {
   public:
    explicit Scanner(std::string source);
    void scanTokens();
    std::vector<Token> getTokens();

   private:
    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(Token::Type type);
    void addToken(Token::Type type, const Value& literal);
    bool match(char expected);
    char peek();
    void string();
    void number();
    char peekNext();
    void identifier();

    static bool isDigit(char c);
    static bool isAlpha(char c);
    static bool isAlphaNumeric(char c);

    // TODO: Checkout how this compiles with constexpr and
    // consteval. We just need to be embedded in the binary.
    // We can also make use of merkel trie to check
    // membership quickly.
    const std::unordered_map<std::string, Token::Type>
        keywords{
            {"and", Token::Type::AND},
            {"class", Token::Type::CLASS},
            {"else", Token::Type::ELSE},
            {"false", Token::Type::FALSE},
            {"for", Token::Type::FOR},
            {"fun", Token::Type::FUN},
            {"if", Token::Type::IF},
            {"nil", Token::Type::NIL},
            {"or", Token::Type::OR},
            {"print", Token::Type::PRINT},
            {"return", Token::Type::RETURN},
            {"super", Token::Type::SUPER},
            {"this", Token::Type::THIS},
            {"true", Token::Type::TRUE},
            {"var", Token::Type::VAR},
            {"while", Token::Type::WHILE},
        };

    const std::string mSource;
    std::vector<Token> mTokens;

    int mLine = 1;
    size_t mCurrent = 0;
    size_t mStart = 0;
};

}  // namespace Lox

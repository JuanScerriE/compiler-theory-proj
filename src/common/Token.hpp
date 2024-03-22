#pragma once

// vought
#include <common/Item.hpp>

// std
#include <optional>

namespace PArL {

class TokenException : public std::exception {
   public:
    explicit TokenException(char const* message)
        : mMessage(message) {
    }
    explicit TokenException(std::string const& message)
        : mMessage(message) {
    }

    [[nodiscard]] char const* what()
        const noexcept override;

   private:
    std::string mMessage;
};

class Token {
   public:
    enum class Type {
        // one or two character tokens
        // LEFT_BRACKET,
        // RIGHT_BRACKET,
        STAR,
        SLASH,
        PLUS,
        MINUS,
        LESS,
        GREATER,
        EQUAL_EQUAL,
        BANG_EQUAL,
        LESS_EQUAL,
        GREATER_EQUAL,
        COMMA,
        LEFT_PAREN,
        RIGHT_PAREN,
        EQUAL,
        COLON,
        SEMICOLON,
        ARROW,
        LEFT_BRACE,
        RIGHT_BRACE,

        // types
        FLOAT_TYPE,
        INTEGER_TYPE,
        BOOL_TYPE,
        COLOR_TYPE,

        // literals
        FLOAT,
        INTEGER,
        COLOR,
        BOOL,
        BUILTIN,
        IDENTIFIER,

        // keywords
        AND,
        OR,
        NOT,
        AS,
        LET,
        RETURN,
        IF,
        ELSE,
        FOR,
        WHILE,
        FUN,

        // special tokens
        COMMENT,
        WHITESPACE,
        END_OF_FILE,
    };

    Token(int line, int column, std::string const& lexeme,
          Type type);

    [[nodiscard]] int getLine() const;
    [[nodiscard]] int getColumn() const;
    [[nodiscard]] std::string getLexeme() const;
    [[nodiscard]] Type getType() const;

    [[nodiscard]] std::string toString() const;

   private:
    [[nodiscard]] bool isContainerType() const;

    void specialiseIfPossible(std::string const& lexeme);

    Value createValue(std::string lexeme);

    int mLine;
    int mColumn;
    std::string mLexeme;
    Type mType;
    Value mValue;
};

}  // namespace PArL

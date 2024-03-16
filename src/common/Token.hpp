#pragma once

// vought
#include <common/Item.hpp>

// std
#include <optional>

namespace Vought {

class TokenException : public std::exception {
   public:
    TokenException(char const* message)
        : mMessage(message) {
    }
    TokenException(std::string message)
        : mMessage(message) {
    }

    [[nodiscard]] char const* what()
        const noexcept override;

   private:
    std::string mMessage;
};

class Token : public Item {
   public:
    enum class Type {
        // one or two character tokens
        DOT,
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

    Token(int line, int column, std::string lexeme,
          Type type)
        : Item(line, column, lexeme),  mType(type) {
        if (isContainerType())
            specialiseIfPossible(lexeme);
    }

    Token(Token const& other)
        : Item(other),
          mType(other.mType),
          mValue(other.mValue) {
    }

    Type getType() const;
    void print(bool withLocation) const override;

   private:
    bool isContainerType() const;
    void specialiseIfPossible(std::string const& lexeme);
    Value createValue(std::string lexeme);

    Type mType;
    std::optional<Value> mValue;
};

}  // namespace Vought

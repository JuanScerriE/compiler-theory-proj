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
        // single character tokens
        COLON,
        COMMA,
        DOT,
        LEFT_BRACE,
        LEFT_PAREN,
        MINUS,
        PLUS,
        RIGHT_BRACE,
        RIGHT_PAREN,
        SEMICOLON,
        SLASH,
        STAR,

        // one or two character tokens
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        EXPONENT,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        RETURN_TYPE,

        // three character token
        SWAP,

        // literals
        FLOAT,
        IDENTIFIER,
        INTEGER,
        STRING,

        // keywords
        AND,
        CLASS,
        ELSE,
        FALSE,
        FOR,
        FUN,
        IF,
        OR,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,

        // special tokens
        COMMENT,
        END_OF_FILE,
        WHITESPACE,
    };

    Token(int line, int column, std::string lexeme,
          Type type)
        : Item(line, column), mType(type) {
        if (isContainerType())
            specialiseIfPossible(lexeme);
    }

    Token(int line, int column, Type type)
        : Item(line, column), mType(type) {
        if (isContainerType())
            throw TokenException(
                "token type requires a lexeme");
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

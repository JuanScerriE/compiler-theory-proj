#pragma once

#include <optional>
#include <ostream>
#include <variant>

namespace Vought {

struct Value {
    enum Type {
        IDENTIFIER,
        INTEGER,
        FLOAT,
    };

    Type type;

    std::variant<std::string, bool, int, float> data;

    static Value createIdentifier(std::string lexeme) {
        return {Type::IDENTIFIER, lexeme};
    }

    static Value createInteger(std::string lexeme) {
        return {Type::INTEGER, std::stoi(lexeme)};
    }

    static Value createFloat(std::string lexeme) {
        return {Type::FLOAT, std::stof(lexeme)};
    }

    [[nodiscard]] std::string toString() const {
        switch (type) {
            case Type::IDENTIFIER:
                return std::get<std::string>(data);
            case Type::INTEGER:
                return std::to_string(std::get<int>(data));
            case Type::FLOAT:
                return std::to_string(
                    std::get<float>(data));

            default:
                return "Undefined Value Type";
        }
    }
};

class Item {
   public:
    Item(int line, int column)
        : mLine(line), mColumn(column) {
    }

    Item(Item const& other) {
        mLine = other.mLine;
        mColumn = other.mColumn;
    }

    [[nodiscard]] int getLine() const;
    [[nodiscard]] int getColumn() const;

    virtual void print(bool withLocation) const = 0;

    friend std::ostream& operator<<(std::ostream& out,
                                    Item const& item);

   protected:
    int mLine;
    int mColumn;
};

class Error : public Item {
   public:
    Error(int line, int column, std::string lexeme,
          std::string message)
        : Item(line, column),
          mLexeme(lexeme),
          mMessage(message) {
    }

    Error(Error const& other)
        : Item(other),
          mLexeme(other.mLexeme),
          mMessage(other.mMessage) {
    }

    void print(bool withLocation) const override;

   private:
    std::string mLexeme;
    std::string mMessage;
};

class Token : public Item {
   public:
    enum class Type {
        // single character tokens
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,
        COLON,

        // one or two character tokens
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        RETURN_TYPE,
        EXPONENT,

        // literals
        IDENTIFIER,
        INTEGER,
        FLOAT,

        // keywords
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,

        // special tokens
        WHITESPACE,
        END_OF_FILE
    };

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

    Token(int line, int column, Type type,
          std::string lexeme)
        : Item(line, column), mType(type) {
        if (isContainerType())
            mValue = createValue(lexeme);
    }

    Token(int line, int column, Type type)
        : Item(line, column), mType(type) {
        if (isContainerType())
            throw TokenException(
                "this token type requires a lexeme");
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

    Value createValue(std::string lexeme);

    Type mType;
    std::optional<Value> mValue;
};

}  // namespace Vought

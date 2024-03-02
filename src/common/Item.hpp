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

    [[nodiscard]] int getLine() const {
        return mLine;
    }

    [[nodiscard]] int getColumn() const {
        return mColumn;
    }

    virtual void print(std::ostream& out) const = 0;

    friend std::ostream& operator<<(std::ostream& out,
                                    Item const& item);

   protected:
    int mLine;
    int mColumn;
};

inline std::ostream& operator<<(std::ostream& out,
                                Item const& item) {
    item.print(out);

    return out;
}

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

    void print(std::ostream& out) const override {
        out << "ERROR(location = " << mLine << ":"
            << mColumn << ", lexeme = {" << mLexeme << "}"
            << ", message = \"" << mMessage << "\")";
    }

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
        STRING,
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

    Token(int line, int column, Type type, Value value)
        : Item(line, column), mType(type), mValue(value) {
    }

    Token(int line, int column, Type type)
        : Item(line, column), mType(type) {
    }

    Token(Token const& other)
        : Item(other),
          mType(other.mType),
          mValue(other.mValue) {
    }

    Type getType() const {
        return mType;
    }

    void print(std::ostream& out) const override {
        switch (mType) {
            case Type::FLOAT:
                out << "FLOAT(" << mValue.value().toString()
                    << ")";
                break;
            case Type::RETURN_TYPE:
                out << "RETURN_TYPE";
                break;
            case Type::EXPONENT:
                out << "EXPONENT";
                break;
            case Type::COLON:
                out << "COLON";
                break;
            case Type::LEFT_PAREN:
                out << "LEFT_PAREN";
                break;
            case Type::RIGHT_PAREN:
                out << "RIGHT_PAREN";
                break;
            case Type::LEFT_BRACE:
                out << "LEFT_BRACE";
                break;
            case Type::RIGHT_BRACE:
                out << "RIGHT_BRACE";
                break;
            case Type::COMMA:
                out << "COMMA";
                break;
            case Type::DOT:
                out << "DOT";
                break;
            case Type::MINUS:
                out << "MINUS";
                break;
            case Type::PLUS:
                out << "PLUS";
                break;
            case Type::SEMICOLON:
                out << "SEMICOLON";
                break;
            case Type::SLASH:
                out << "SLASH";
                break;
            case Type::STAR:
                out << "STAR";
                break;
            case Type::BANG:
                out << "BANG";
                break;
            case Type::BANG_EQUAL:
                out << "BANG_EQUAL";
                break;
            case Type::EQUAL:
                out << "EQUAL";
                break;
            case Type::EQUAL_EQUAL:
                out << "EQUAL_EQUAL";
                break;
            case Type::GREATER:
                out << "GREATER";
                break;
            case Type::GREATER_EQUAL:
                out << "GREATER_EQUAL";
                break;
            case Type::LESS:
                out << "LESS";
                break;
            case Type::LESS_EQUAL:
                out << "LESS_EQUAL";
                break;
            case Type::IDENTIFIER:
                out << "IDENTIFIER("
                    << mValue.value().toString() << ")" << "location = " << mLine << ":" << mColumn;
                break;
            case Type::STRING:
                out << "STRING("
                    << mValue.value().toString() << ")";
                break;
            case Type::INTEGER:
                out << "INTEGER("
                    << mValue.value().toString() << ")";
                break;
            case Type::AND:
                out << "AND";
                break;
            case Type::CLASS:
                out << "CLASS";
                break;
            case Type::ELSE:
                out << "ELSE";
                break;
            case Type::FALSE:
                out << "FALSE";
                break;
            case Type::FUN:
                out << "FUN";
                break;
            case Type::FOR:
                out << "FOR";
                break;
            case Type::IF:
                out << "IF";
                break;
            case Type::NIL:
                out << "NIL";
                break;
            case Type::OR:
                out << "OR";
                break;
            case Type::PRINT:
                out << "PRINT";
                break;
            case Type::RETURN:
                out << "RETURN";
                break;
            case Type::SUPER:
                out << "SUPER";
                break;
            case Type::THIS:
                out << "THIS";
                break;
            case Type::TRUE:
                out << "TRUE";
                break;
            case Type::VAR:
                out << "VAR";
                break;
            case Type::WHILE:
                out << "WHILE";
                break;
            case Type::WHITESPACE:
                out << "WHITESPACE";
                break;
            case Type::END_OF_FILE:
                out << "END_OF_FILE";
                break;
        }
    }

   private:
    Type mType;
    std::optional<Value> mValue;
};

}  // namespace Vought

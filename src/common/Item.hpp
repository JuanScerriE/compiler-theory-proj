#pragma once

#include <ostream>
#include <variant>

namespace Vought {

struct Value {
    enum Type {
        NIL,
        BOOL,
        STRING,
        NUMBER,
    };

    Type type;

    std::variant<std::monostate, bool, std::string, double>
        data;

    static Value createNil() {
        return {Type::NIL, std::monostate{}};
    }

    static Value createBool(bool boolean) {
        return {Type::BOOL, boolean};
    }

    static Value createString(std::string string) {
        return {Type::STRING, string};
    }

    static Value createNumber(double number) {
        return {Type::NUMBER, number};
    }

    [[nodiscard]] std::string toString() const {
        switch (type) {
            case Type::NIL:
                return "nil";
            case Type::BOOL:
                return std::get<bool>(data) ? "true"
                                            : "false";
            case Type::NUMBER:
                return std::to_string(
                    std::get<double>(data));
            case Type::STRING:
                return "\"" + std::get<std::string>(data) +
                       "\"";
            default:
                return "Undefined Value Type";
        }
    }
};

class Item {
   public:
    Item(std::string lexeme, int line)
        : mLexeme(lexeme), mLine(line) {
    }

    Item(Item const& other) {
        mLexeme = other.mLexeme;
        mLine = other.mLine;
    }

    [[nodiscard]] int getLine() const {
        return mLine;
    }

    virtual void print(std::ostream& out) const = 0;

    friend std::ostream& operator<<(std::ostream& out,
                                    Item const& item);

   protected:
    std::string mLexeme;
    int mLine;
};

inline std::ostream& operator<<(std::ostream& out,
                                Item const& item) {
    item.print(out);

    return out;
}

class Error : public Item {
   public:
    Error(std::string lexeme, int line, std::string message)
        : Item(lexeme, line), mMessage(message) {
    }

    Error(Error const& other)
        : Item(other), mMessage(other.mMessage) {
    }

    void print(std::ostream& out) const override {
        out << "ERROR(line = " << mLine << ", lexeme = {"
            << mLexeme << "}"
            << ", message = \"" << mMessage << "\")";
    }

   private:
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

        // one or two character tokens
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // literals
        IDENTIFIER,
        STRING,
        NUMBER,

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

    Token(std::string lexeme, int line, Type type,
          Value literal)
        : Item(lexeme, line),
          mType(type),
          mLiteral(literal) {
    }

    Token(Token const& other)
        : Item(other),
          mType(other.mType),
          mLiteral(other.mLiteral) {
    }

    Type getType() const {
        return mType;
    }

    void print(std::ostream& out) const override {
        switch (mType) {
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
                out << "IDENTIFIER(" << mLexeme << ")";
                break;
            case Type::STRING:
                out << "STRING(" << mLiteral.toString()
                    << ")";
                break;
            case Type::NUMBER:
                out << "NUMBER(" << mLiteral.toString()
                    << ")";
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
    Value mLiteral;
};

}  // namespace Vought

#pragma once

// std
#include <ios>
#include <string>
#include <variant>

namespace Vought {

struct Value {
    enum Type {
        NIL,
        BOOL,
        STRING,
        NUMBER,
        LEXER_ERROR,
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

}  // namespace Lox

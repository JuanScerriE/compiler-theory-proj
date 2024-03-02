#pragma once

// std
#include <string>
#include <variant>

namespace Vought {

struct Value {
    enum Type {
        NIL,
        BOOL,
        INTEGER,
        FLOAT,
    };

    Type type;

    std::variant<std::monostate, bool, int, float> data;

    static Value createNil() {
        return {Type::NIL, std::monostate{}};
    }

    static Value createBool(bool boolean) {
        return {Type::BOOL, boolean};
    }

    static Value createInteger(std::string lexeme) {
        return {Type::INTEGER, std::stoi(lexeme)};
    }

    static Value createFloat(std::string lexeme) {
        return {Type::FLOAT, std::stof(lexeme)};
    }

    [[nodiscard]] std::string toString() const {
        switch (type) {
            case Type::NIL:
                return "nil";
            case Type::BOOL:
                return std::get<bool>(data) ? "true"
                                            : "false";
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

}  // namespace Vought

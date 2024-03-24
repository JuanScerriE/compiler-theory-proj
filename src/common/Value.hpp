#pragma once

// std
#include <cstdint>
#include <string>
#include <variant>

// parl
#include <common/Abort.hpp>

namespace PArL {

struct Color {
    [[maybe_unused]] uint8_t mR;
    [[maybe_unused]] uint8_t mG;
    [[maybe_unused]] uint8_t mB;
};

enum class Builtin {
    WIDTH,
    HEIGHT,
    READ,
    RANDOM_INT,
    PRINT,
    DELAY,
    WRITE,
    WRITE_BOX,
    CLEAR,
};

using Identifier = std::string;

struct Value {
    enum class Type {
        FLOAT,
        INTEGER,
        COLOR,
        BOOL,
        BUILTIN,
        IDENTIFIER,
    };

    [[nodiscard]] static Value createFloat(
        std::string const& lexeme);
    [[nodiscard]] static Value createInteger(
        std::string const& lexeme);
    [[nodiscard]] static Value createColor(
        std::string const& lexeme);
    [[nodiscard]] static Value createBool(
        std::string const& lexeme);
    [[nodiscard]] static Value createBuiltin(
        std::string const& lexeme);
    [[nodiscard]] static Value createIdentifier(
        std::string const& lexeme);

    template <typename T>
    T as() const {
        if (std::holds_alternative<T>(mData)) {
            return std::get<T>(mData);
        }

        abort("accessing value as inappropriate type");
    }

    Type mType;

    std::variant<float, int, Color, bool, Builtin,
                 Identifier>
        mData;
};

}  // namespace PArL

#pragma once

// std
#include <cstdint>
#include <string>
#include <variant>
#include <unordered_map>

namespace Vought {

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

enum Builtin {
    __WIDTH,
    __HEIGHT,
    __READ,
    __RANDOM_INT,
    __PRINT,
    __DELAY,
    __WRITE,
    __WRITE_BOX,
};

static const std::unordered_map<std::string, Builtin>
    builtins{
        {"__width", __WIDTH},
        {"__height", __HEIGHT},
        {"__read", __READ},
        {"__random_int", __RANDOM_INT},
        {"__print", __PRINT},
        {"__delay", __DELAY},
        {"__write", __WRITE},
        {"__write_box", __WRITE_BOX},
    };

class ValueException : public std::exception {
   public:
    ValueException(char const* message)
        : mMessage(message) {
    }
    ValueException(std::string message)
        : mMessage(message) {
    }

    [[nodiscard]] char const* what()
        const noexcept override;

   private:
    std::string mMessage;
};

struct Value {
    enum Type {
        FLOAT,
        INTEGER,
        COLOR,
        BOOL,
        BUILTIN,
        IDENTIFIER,
    };


    // NOTE: here we make an exception to the naming
    // convetion as we are using a struct instead of a
    // class.
    Type type;

    std::variant<float, int, Color, bool, Builtin, std::string> data;

    static Value createFloat(std::string const& lexeme);
    static Value createInteger(std::string const& lexeme);
    static Value createColor(std::string const& lexeme);
    static Value createBool(std::string const& lexeme);
    static Value createBuiltin(
        std::string const& lexeme);
    static Value createIdentifier(
        std::string const& lexeme);

    [[nodiscard]] std::string toString() const;
};

}  // namespace Vought

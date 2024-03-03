#pragma once

// std
#include <string>
#include <variant>

namespace Vought {

struct Value {
    enum Type {
        FLOAT,
        IDENTIFIER,
        INTEGER,
    };

    // NOTE: here we make an exception to the naming
    // convetion as we are using a struct instead of a
    // class.
    Type type;
    std::variant<std::string, bool, int, float> data;

    static Value createFloat(std::string const& lexeme);

    static Value createIdentifier(
        std::string const& lexeme);

    static Value createInteger(std::string const& lexeme);

    [[nodiscard]] std::string toString() const;
};

}  // namespace Vought

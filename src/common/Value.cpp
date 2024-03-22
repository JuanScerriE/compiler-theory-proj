// vought
#include <common/Value.hpp>

// fmt
#include <fmt/core.h>

// std
#include <string>

namespace PArL {

char const* ValueException::what() const noexcept {
    return mMessage.c_str();
}

Value Value::createFloat(std::string const& lexeme) {
    return {Type::FLOAT, std::stof(lexeme)};
}

Value Value::createInteger(std::string const& lexeme) {
    return {Type::INTEGER, std::stoi(lexeme)};
}

Value Value::createColor(std::string const& lexeme) {
    Color color{};

    color.r += lexeme[1];
    color.r += lexeme[2];
    color.g += lexeme[3];
    color.g += lexeme[4];
    color.b += lexeme[5];
    color.b += lexeme[6];

    return {Type::COLOR, color};
}

Value Value::createBool(std::string const& lexeme) {
    return {Type::BOOL, (lexeme == "true") ? true : false};
}

Value Value::createBuiltin(std::string const& lexeme) {
    if (builtins.count(lexeme) > 0) {
        return {Type::BUILTIN, builtins.at(lexeme)};
    }

    // TODO: do not crash on undefined but report as lexical
    // error (since technically builtins should be keywords)
    throw ValueException(fmt::format(
        "usage of undefined builtin {}", lexeme));
}

Value Value::createIdentifier(std::string const& lexeme) {
    return {Type::IDENTIFIER, lexeme};
}

std::string Value::toString() const {
    switch (type) {
        case Type::FLOAT:
            return std::to_string(std::get<float>(data));
        case Type::INTEGER:
            return std::to_string(std::get<int>(data));
        case Type::COLOR: {
            Color color = std::get<Color>(data);

            return fmt::format("#{:x}{:x}{:x}", color.r,
                               color.g, color.b);
        }
        case Type::BOOL:
            return std::get<bool>(data) ? "true" : "false";
        case Type::BUILTIN:
            for (auto [str, builtin] : builtins) {
                if (std::get<Builtin>(data) == builtin) {
                    return str;
                }
            }
            /* unreachable */
        case Type::IDENTIFIER:
            return std::get<std::string>(data);
        default:
            return "Undefined Value Type";
    }
}

}  // namespace PArL

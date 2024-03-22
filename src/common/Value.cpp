// parl
#include <common/Abort.hpp>
#include <common/Errors.hpp>
#include <common/Value.hpp>

// fmt
#include <fmt/core.h>

// std
#include <string>
#include <unordered_map>

namespace PArL {

static const std::unordered_map<std::string, Builtin>
    builtins{
        {"__width", Builtin::WIDTH},
        {"__height", Builtin::HEIGHT},
        {"__read", Builtin::READ},
        {"__random_int", Builtin::RANDOM_INT},
        {"__print", Builtin::PRINT},
        {"__delay", Builtin::DELAY},
        {"__write", Builtin::WRITE},
        {"__write_box", Builtin::WRITE_BOX},
        {"__clear", Builtin::CLEAR},
    };

Value Value::createFloat(std::string const& lexeme) {
    return {Type::FLOAT, std::stof(lexeme)};
}

Value Value::createInteger(std::string const& lexeme) {
    return {Type::INTEGER, std::stoi(lexeme)};
}

Value Value::createColor(std::string const& lexeme) {
    Color color{};

    color.mR = lexeme[1] + lexeme[2];
    color.mG = lexeme[3] + lexeme[4];
    color.mB = lexeme[5] + lexeme[6];

    return {Type::COLOR, color};
}

Value Value::createBool(std::string const& lexeme) {
    return {Type::BOOL, lexeme == "true"};
}

Value Value::createBuiltin(std::string const& lexeme) {
    if (builtins.count(lexeme) > 0) {
        return {Type::BUILTIN, builtins.at(lexeme)};
    }

    throw UndefinedBuiltin(fmt::format(
        "usage of undefined builtin {}", lexeme));
}

Value Value::createIdentifier(std::string const& lexeme) {
    return {Type::IDENTIFIER, lexeme};
}

}  // namespace PArL

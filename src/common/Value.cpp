// vought
#include <common/Value.hpp>

// std
#include <string>

namespace Vought {

Value Value::createString(std::string const& lexeme) {
    return {Type::STRING,
            lexeme.substr(1, lexeme.length() - 2)};
}

Value Value::createFloat(std::string const& lexeme) {
    return {Type::FLOAT, std::stof(lexeme)};
}

Value Value::createIdentifier(std::string const& lexeme) {
    return {Type::IDENTIFIER, lexeme};
}

Value Value::createInteger(std::string const& lexeme) {
    return {Type::INTEGER, std::stoi(lexeme)};
}

std::string Value::toString() const {
    switch (type) {
        case Type::FLOAT:
            return std::to_string(std::get<float>(data));
        case Type::IDENTIFIER:
            return std::get<std::string>(data);
        case Type::INTEGER:
            return std::to_string(std::get<int>(data));
        case Type::STRING:
            return std::get<std::string>(data);
        default:
            return "Undefined Value Type";
    }
}

}  // namespace Vought

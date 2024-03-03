#include <common/Value.hpp>

namespace Vought {

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
        default:
            return "Undefined Value Type";
    }
}

}  // namespace Vought

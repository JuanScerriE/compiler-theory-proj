#include <fmt/core.h>

// vought
#include <common/Token.hpp>

// std
#include <unordered_map>

namespace Vought {

char const* TokenException::what() const noexcept {
    return mMessage.c_str();
}

static const std::unordered_map<std::string, Token::Type>
    keywords{
        {"and", Token::Type::AND},
        {"class", Token::Type::CLASS},
        {"else", Token::Type::ELSE},
        {"false", Token::Type::FALSE},
        {"for", Token::Type::FOR},
        {"fun", Token::Type::FUN},
        {"if", Token::Type::IF},
        {"or", Token::Type::OR},
        {"return", Token::Type::RETURN},
        {"super", Token::Type::SUPER},
        {"this", Token::Type::THIS},
        {"true", Token::Type::TRUE},
        {"var", Token::Type::VAR},
        {"while", Token::Type::WHILE},
    };

Token::Type Token::getType() const {
    return mType;
}

void Token::print(bool withLocation) const {
    if (withLocation)
        fmt::print("{}:{} ", mLine, mColumn);

    switch (mType) {
        case Type::FLOAT:
            fmt::println("FLOAT({})",
                         mValue.value().toString());
            break;
        case Type::IDENTIFIER:
            fmt::println("IDENTIFIER({})",
                         mValue.value().toString());
            break;
        case Type::STRING:
            fmt::println("STRING({})",
                         mValue.value().toString());
            break;
        case Type::INTEGER:
            fmt::println("INTEGER({})",
                         mValue.value().toString());
            break;
        case Type::RETURN_TYPE:
            fmt::println("RETURN_TYPE");
            break;
        case Type::COMMENT:
            fmt::println("COMMENT");
            break;
        case Type::EXPONENT:
            fmt::println("EXPONENT");
            break;
        case Type::COLON:
            fmt::println("COLON");
            break;
        case Type::LEFT_PAREN:
            fmt::println("LEFT_PAREN");
            break;
        case Type::RIGHT_PAREN:
            fmt::println("RIGHT_PAREN");
            break;
        case Type::LEFT_BRACE:
            fmt::println("LEFT_BRACE");
            break;
        case Type::RIGHT_BRACE:
            fmt::println("RIGHT_BRACE");
            break;
        case Type::COMMA:
            fmt::println("COMMA");
            break;
        case Type::DOT:
            fmt::println("DOT");
            break;
        case Type::MINUS:
            fmt::println("MINUS");
            break;
        case Type::PLUS:
            fmt::println("PLUS");
            break;
        case Type::SEMICOLON:
            fmt::println("SEMICOLON");
            break;
        case Type::SLASH:
            fmt::println("SLASH");
            break;
        case Type::STAR:
            fmt::println("STAR");
            break;
        case Type::BANG:
            fmt::println("BANG");
            break;
        case Type::BANG_EQUAL:
            fmt::println("BANG_EQUAL");
            break;
        case Type::EQUAL:
            fmt::println("EQUAL");
            break;
        case Type::EQUAL_EQUAL:
            fmt::println("EQUAL_EQUAL");
            break;
        case Type::GREATER:
            fmt::println("GREATER");
            break;
        case Type::GREATER_EQUAL:
            fmt::println("GREATER_EQUAL");
            break;
        case Type::LESS:
            fmt::println("LESS");
            break;
        case Type::LESS_EQUAL:
            fmt::println("LESS_EQUAL");
            break;
        case Type::AND:
            fmt::println("AND");
            break;
        case Type::CLASS:
            fmt::println("CLASS");
            break;
        case Type::ELSE:
            fmt::println("ELSE");
            break;
        case Type::FALSE:
            fmt::println("FALSE");
            break;
        case Type::FUN:
            fmt::println("FUN");
            break;
        case Type::FOR:
            fmt::println("FOR");
            break;
        case Type::IF:
            fmt::println("IF");
            break;
        case Type::OR:
            fmt::println("OR");
            break;
        case Type::RETURN:
            fmt::println("RETURN");
            break;
        case Type::SUPER:
            fmt::println("SUPER");
            break;
        case Type::THIS:
            fmt::println("THIS");
            break;
        case Type::TRUE:
            fmt::println("TRUE");
            break;
        case Type::VAR:
            fmt::println("VAR");
            break;
        case Type::WHILE:
            fmt::println("WHILE");
            break;
        case Type::WHITESPACE:
            fmt::println("WHITESPACE");
            break;
        case Type::SWAP:
            fmt::println("SWAP");
            break;
        case Type::END_OF_FILE:
            fmt::println("END_OF_FILE");
            break;
    }
}

bool Token::isContainerType() const {
    switch (mType) {
        case Type::STRING:
            /* fall through */
        case Type::FLOAT:
            /* fall through */
        case Type::IDENTIFIER:
            /* fall through */
        case Type::INTEGER:
            return true;

        default:
            return false;
    }
}

void Token::specialiseIfPossible(
    std::string const& lexeme) {
    if (mType == Token::Type::IDENTIFIER &&
        keywords.count(lexeme) > 0) {
        mType = keywords.at(lexeme);

        return;
    }

    mValue = createValue(lexeme);
}

Value Token::createValue(std::string lexeme) {
    switch (mType) {
        case Type::STRING:
            return Value::createString(lexeme);
        case Type::FLOAT:
            return Value::createFloat(lexeme);
        case Type::IDENTIFIER:
            return Value::createIdentifier(lexeme);
        case Type::INTEGER:
            return Value::createInteger(lexeme);

        default:
            throw TokenException(
                "token is not a container type");
    }
}

}  // namespace Vought

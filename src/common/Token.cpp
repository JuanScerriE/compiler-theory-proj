#include <fmt/core.h>

// vought
#include <common/Token.hpp>

// std
#include <unordered_map>
#include "common/Value.hpp"

namespace Vought {

char const* TokenException::what() const noexcept {
    return mMessage.c_str();
}

static const std::unordered_map<std::string, Token::Type>
    keywords{
        {"float", Token::Type::FLOAT_TYPE},
        {"int", Token::Type::INTEGER_TYPE},
        {"bool", Token::Type::BOOL_TYPE},
        {"color", Token::Type::COLOR_TYPE},
        {"and", Token::Type::AND},
        {"or", Token::Type::OR},
        {"not", Token::Type::NOT},
        {"as", Token::Type::AS},
        {"let", Token::Type::LET},
        {"return", Token::Type::RETURN},
        {"if", Token::Type::IF},
        {"else", Token::Type::ELSE},
        {"for", Token::Type::FOR},
        {"while", Token::Type::WHILE},
        {"fun", Token::Type::FUN},
    };

static const std::unordered_map<std::string, Token::Type>
    keywordLiterals{
        {"true", Token::Type::BOOL},
        {"false", Token::Type::BOOL},
    };

Token::Type Token::getType() const {
    return mType;
}

std::optional<Value> Token::getValue() const {
    return mValue;
}


std::string Token::toString(bool withLocation) const {
    std::string str = "";

    if (withLocation)
        str += fmt::format("{}:{} ", mLine, mColumn);

    switch (mType) {
        case Type::FLOAT:
            str += fmt::format("FLOAT({})",
                         mValue.value().toString());
            break;
        case Type::INTEGER:
            str += fmt::format("INTEGER({})",
                         mValue.value().toString());
            break;
        case Type::COLOR:
            str += fmt::format("COLOR({})",
                         mValue.value().toString());
            break;
        case Type::BOOL:
            str += fmt::format("BOOL({})",
                         mValue.value().toString());
            break;
        case Type::BUILTIN:
            str += fmt::format("BUILTIN({})",
                         mValue.value().toString());
            break;
        case Type::IDENTIFIER:
            str += fmt::format("IDENTIFIER({})",
                         mValue.value().toString());

            break;
        case Type::DOT:
            str += "DOT";
            break;
        case Type::STAR:
            str += "STAR";
            break;
        case Type::SLASH:
            str += "SLASH";
            break;
        case Type::PLUS:
            str += "PLUS";
            break;
        case Type::MINUS:
            str += "MINUS";
            break;
        case Type::LESS:
            str += "LESS";
            break;
        case Type::GREATER:
            str += "GREATER";
            break;
        case Type::EQUAL_EQUAL:
            str += "EQUAL_EQUAL";
            break;
        case Type::BANG_EQUAL:
            str += "BANG_EQUAL";
            break;
        case Type::GREATER_EQUAL:
            str += "GREATER_EQUAL";
            break;
        case Type::LESS_EQUAL:
            str += "LESS_EQUAL";
            break;
        case Type::COMMA:
            str += "COMMA";
            break;
        case Type::LEFT_PAREN:
            str += "LEFT_PAREN";
            break;
        case Type::RIGHT_PAREN:
            str += "RIGHT_PAREN";
            break;
        case Type::EQUAL:
            str += "EQUAL";
            break;
        case Type::COLON:
            str += "COLON";
            break;
        case Type::SEMICOLON:
            str += "SEMICOLON";
            break;
        case Type::ARROW:
            str += "ARROW";
            break;
        case Type::LEFT_BRACE:
            str += "LEFT_BRACE";
            break;
        case Type::RIGHT_BRACE:
            str += "RIGHT_BRACE";
            break;

        case Type::FLOAT_TYPE:
            str += "FLOAT_TYPE";
            break;
        case Type::INTEGER_TYPE:
            str += "INTEGER_TYPE";
            break;
        case Type::BOOL_TYPE:
            str += "BOOL_TYPE";
            break;
        case Type::COLOR_TYPE:
            str += "COLOR_TYPE";
            break;

        case Type::AND:
            str += "ELSE";
            break;
        case Type::OR:
            str += "FALSE";
            break;
        case Type::NOT:
            str += "NOT";
            break;
        case Type::AS:
            str += "AS";
            break;
        case Type::LET:
            str += "LET";
            break;
        case Type::RETURN:
            str += "RETURN";
            break;
        case Type::IF:
            str += "RETURN";
            break;
        case Type::ELSE:
            str += "RETURN";
            break;
        case Type::FOR:
            str += "FOR";
            break;
        case Type::WHILE:
            str += "WHILE";
            break;
        case Type::FUN:
            str += "FUN";
            break;

        case Type::COMMENT:
            str += "COMMENT";
            break;
        case Type::WHITESPACE:
            str += "WHITESPACE";
            break;
        case Type::END_OF_FILE:
            str += "END_OF_FILE";
            break;
    }

    return str;
}

bool Token::isContainerType() const {
    switch (mType) {
        case Type::FLOAT:
            /* fall through */
        case Type::INTEGER:
            /* fall through */
        case Type::BOOL:
            /* fall through */
        case Type::COLOR:
            /* fall through */
        case Type::BUILTIN:
            /* fall through */
        case Type::IDENTIFIER:
            /* fall through */
            return true;

        default:
            return false;
    }
}

void Token::specialiseIfPossible(
    std::string const& lexeme) {
    if (mType == Token::Type::IDENTIFIER) {
        if (keywords.count(lexeme) > 0) {
            mType = keywords.at(lexeme);

            return;
        }

        if (keywordLiterals.count(lexeme) > 0) {
            mType = keywordLiterals.at(lexeme);
        }
    }

    mValue = createValue(lexeme);
}

Value Token::createValue(std::string lexeme) {
    switch (mType) {
        case Type::FLOAT:
            return Value::createFloat(lexeme);
        case Type::INTEGER:
            return Value::createInteger(lexeme);
        case Type::BOOL:
            return Value::createBool(lexeme);
        case Type::COLOR:
            return Value::createColor(lexeme);
        case Type::BUILTIN:
            return Value::createBuiltin(lexeme);
        case Type::IDENTIFIER:
            return Value::createIdentifier(lexeme);

        default:
            throw TokenException(
                "token is not a container type");
    }
}

}  // namespace Vought

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

void Token::print(bool withLocation) const {
    if (withLocation)
        fmt::print("{}:{} ", mLine, mColumn);

    switch (mType) {
        case Type::FLOAT:
            fmt::println("FLOAT({})",
                         mValue.value().toString());
            break;
        case Type::INTEGER:
            fmt::println("INTEGER({})",
                         mValue.value().toString());
            break;
        case Type::COLOR:
            fmt::println("COLOR({})",
                         mValue.value().toString());
            break;
        case Type::BOOL:
            fmt::println("BOOL({})",
                         mValue.value().toString());
            break;
        case Type::BUILTIN:
            fmt::println("BUILTIN({})",
                         mValue.value().toString());
            break;
        case Type::IDENTIFIER:
            fmt::println("IDENTIFIER({})",
                         mValue.value().toString());
            break;

        case Type::DOT:
            fmt::println("DOT");
            break;
        case Type::STAR:
            fmt::println("STAR");
            break;
        case Type::SLASH:
            fmt::println("SLASH");
            break;
        case Type::PLUS:
            fmt::println("PLUS");
            break;
        case Type::MINUS:
            fmt::println("MINUS");
            break;
        case Type::LESS:
            fmt::println("LESS");
            break;
        case Type::GREATER:
            fmt::println("GREATER");
            break;
        case Type::EQUAL_EQUAL:
            fmt::println("EQUAL_EQUAL");
            break;
        case Type::BANG_EQUAL:
            fmt::println("BANG_EQUAL");
            break;
        case Type::GREATER_EQUAL:
            fmt::println("GREATER_EQUAL");
            break;
        case Type::LESS_EQUAL:
            fmt::println("LESS_EQUAL");
            break;
        case Type::COMMA:
            fmt::println("COMMA");
            break;
        case Type::LEFT_PAREN:
            fmt::println("LEFT_PAREN");
            break;
        case Type::RIGHT_PAREN:
            fmt::println("RIGHT_PAREN");
            break;
        case Type::EQUAL:
            fmt::println("EQUAL");
            break;
        case Type::COLON:
            fmt::println("COLON");
            break;
        case Type::SEMICOLON:
            fmt::println("SEMICOLON");
            break;
        case Type::ARROW:
            fmt::println("ARROW");
            break;
        case Type::LEFT_BRACE:
            fmt::println("LEFT_BRACE");
            break;
        case Type::RIGHT_BRACE:
            fmt::println("RIGHT_BRACE");
            break;

        case Type::FLOAT_TYPE:
            fmt::println("FLOAT_TYPE");
            break;
        case Type::INTEGER_TYPE:
            fmt::println("INTEGER_TYPE");
            break;
        case Type::BOOL_TYPE:
            fmt::println("BOOL_TYPE");
            break;
        case Type::COLOR_TYPE:
            fmt::println("COLOR_TYPE");
            break;

        case Type::AND:
            fmt::println("ELSE");
            break;
        case Type::OR:
            fmt::println("FALSE");
            break;
        case Type::NOT:
            fmt::println("NOT");
            break;
        case Type::AS:
            fmt::println("AS");
            break;
        case Type::LET:
            fmt::println("LET");
            break;
        case Type::RETURN:
            fmt::println("RETURN");
            break;
        case Type::IF:
            fmt::println("RETURN");
            break;
        case Type::ELSE:
            fmt::println("RETURN");
            break;
        case Type::FOR:
            fmt::println("FOR");
            break;
        case Type::WHILE:
            fmt::println("WHILE");
            break;
        case Type::FUN:
            fmt::println("FUN");
            break;

        case Type::COMMENT:
            fmt::println("COMMENT");
            break;
        case Type::WHITESPACE:
            fmt::println("WHITESPACE");
            break;
        case Type::END_OF_FILE:
            fmt::println("END_OF_FILE");
            break;
    }
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

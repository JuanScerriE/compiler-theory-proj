#include <fmt/core.h>

// parl
#include <common/Abort.hpp>
#include <common/Token.hpp>
#include <common/Value.hpp>

// std
#include <unordered_map>

namespace PArL {

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

static const std::unordered_map<std::string, Token::Type>
    keywordBuiltins{
        {"__true", Token::Type::BOOL},
        {"__false", Token::Type::BOOL},
    };

Token::Token(int line, int column,
             std::string const& lexeme, Type type)
    : mLine(line),
      mColumn(column),
      mLexeme(lexeme),
      mType(type) {
    if (isContainerType())
        specialiseIfPossible(lexeme);
}

int Token::getLine() const {
    return mLine;
}

int Token::getColumn() const {
    return mColumn;
}

std::string Token::getLexeme() const {
    return mLexeme;
}

Token::Type Token::getType() const {
    return mType;
}

std::string Token::toString() const {
    switch (mType) {
        case Type::STAR:
            return "STAR";
        case Type::SLASH:
            return "SLASH";
        case Type::PLUS:
            return "PLUS";
        case Type::MINUS:
            return "MINUS";
        case Type::LESS:
            return "LESS";
        case Type::GREATER:
            return "GREATER";
        case Type::EQUAL_EQUAL:
            return "EQUAL_EQUAL";
        case Type::BANG_EQUAL:
            return "BANG_EQUAL";
        case Type::GREATER_EQUAL:
            return "GREATER_EQUAL";
        case Type::LESS_EQUAL:
            return "LESS_EQUAL";
        case Type::COMMA:
            return "COMMA";
        case Type::LEFT_PAREN:
            return "LEFT_PAREN";
        case Type::RIGHT_PAREN:
            return "RIGHT_PAREN";
        case Type::EQUAL:
            return "EQUAL";
        case Type::COLON:
            return "COLON";
        case Type::SEMICOLON:
            return "SEMICOLON";
        case Type::ARROW:
            return "ARROW";
        case Type::LEFT_BRACE:
            return "LEFT_BRACE";
        case Type::RIGHT_BRACE:
            return "RIGHT_BRACE";

        case Type::FLOAT_TYPE:
            return "FLOAT_TYPE";
        case Type::INTEGER_TYPE:
            return "INTEGER_TYPE";
        case Type::BOOL_TYPE:
            return "BOOL_TYPE";
        case Type::COLOR_TYPE:
            return "COLOR_TYPE";

        case Type::FLOAT:
            return fmt::format("FLOAT({})", mLexeme);
        case Type::INTEGER:
            return fmt::format("INTEGER({})", mLexeme);
        case Type::COLOR:
            return fmt::format("COLOR({})", mLexeme);
        case Type::BOOL:
            return fmt::format("BOOL({})", mLexeme);
        case Type::BUILTIN:
            return fmt::format("BUILTIN({})", mLexeme);
        case Type::IDENTIFIER:
            return fmt::format("IDENTIFIER({})", mLexeme);

        case Type::AND:
            return "ELSE";
        case Type::OR:
            return "FALSE";
        case Type::NOT:
            return "NOT";
        case Type::AS:
            return "AS";
        case Type::LET:
            return "LET";
        case Type::RETURN:
            return "RETURN";
        case Type::IF:
            return "IF";
        case Type::ELSE:
            return "ELSE";
        case Type::FOR:
            return "FOR";
        case Type::WHILE:
            return "WHILE";
        case Type::FUN:
            return "FUN";

        case Type::COMMENT:
            return "COMMENT";
        case Type::WHITESPACE:
            return "WHITESPACE";
        case Type::END_OF_FILE:
            return "END_OF_FILE";
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
    switch (mType) {
        case Type::FLOAT:
            mValue = Value::createFloat(lexeme);
            break;
        case Type::INTEGER:
            mValue = Value::createInteger(lexeme);
            break;
        case Type::BOOL:
            mValue = Value::createBool(lexeme);
            break;
        case Type::COLOR:
            mValue = Value::createColor(lexeme);
            break;
        case Type::BUILTIN:
            mValue = Value::createBuiltin(lexeme);
            break;
        case Type::IDENTIFIER:
            if (keywords.count(lexeme) > 0) {
                mType = keywords.at(lexeme);
                break;
            } else {
            }
            break;
        default:
            ABORT("unreachable");
    }
    if (mType == Type::IDENTIFIER) {
    }
    /*
                mValue = Value::createIdentifier(lexeme);
        }
        if (mType == Type::IDENTIFIER) {
            if (keywords.count(lexeme) > 0) {
                mType = keywords.at(lexeme);

                return;
            }

            if (keywordLiterals.count(lexeme) > 0) {
                mType = keywordLiterals.at(lexeme);
            }
        }

        mValue = createValue(lexeme);
        */
}

/*
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
*/

}  // namespace PArL

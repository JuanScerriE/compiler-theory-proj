#include <fmt/core.h>

#include <common/Item.hpp>

namespace Vought {

int Item::getLine() const {
    return mLine;
}

int Item::getColumn() const {
    return mColumn;
}

void Error::print(bool withLocation) const {
    if (withLocation)
        fmt::print("{}:{} ", mLine, mColumn);

    fmt::println(
        "ERROR(lexeme = \"{}\", message "
        "= \"{}\")",
        mLexeme, mMessage);
}

char const* Token::TokenException::what() const noexcept {
    return mMessage.c_str();
}

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
        case Type::INTEGER:
            fmt::println("INTEGER({})",
                         mValue.value().toString());
            break;
        case Type::RETURN_TYPE:
            fmt::println("RETURN_TYPE");
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
        case Type::NIL:
            fmt::println("NIL");
            break;
        case Type::OR:
            fmt::println("OR");
            break;
        case Type::PRINT:
            fmt::println("PRINT");
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
        case Type::FLOAT:
            /* fall through */
        case Type::IDENTIFIER:
            /* fall through */
        case Type::INTEGER:
            return true;
    }

    return false;
}

std::optional<Token::Type> Token::findKeyword(
    std::string& lexeme) const {
    if (mKeywords.find(lexeme) == mKeywords.end()) {
        return {};
    }

    return mKeywords.at(lexeme);
}

Value Token::createValue(std::string lexeme) {
    switch (mType) {
        case Type::INTEGER:
            return Value::createInteger(lexeme);
        case Type::FLOAT:
            return Value::createFloat(lexeme);
        case Type::IDENTIFIER:
            return Value::createIdentifier(lexeme);
    }

    throw TokenException(
        "token type is not a container type");
}

}  // namespace Vought

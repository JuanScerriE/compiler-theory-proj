// std
#include <iostream>
#include <utility>

// lox
#include <common/Token.hpp>

namespace Lox {

Token::Token(Type type, std::string lexeme, Value literal,
             int line)
    : mType(type),
      mLiteral(std::move(literal)),
      mLexeme(std::move(lexeme)),
      mLine(line) {
}

Token::Token(Token const& token) {
    mType = token.mType;
    mLine = token.mLine;
    mLiteral = token.mLiteral;
    mLexeme = token.mLexeme;
}

std::ostream& operator<<(std::ostream& out,
                         Token const& token) {
    switch (token.mType) {
        case Token::Type::LEFT_PAREN:
            out << "LEFT_PAREN " << token.mLexeme;
            break;
        case Token::Type::RIGHT_PAREN:
            out << "RIGHT_PAREN " << token.mLexeme;
            break;
        case Token::Type::LEFT_BRACE:
            out << "LEFT_BRACE " << token.mLexeme;
            break;
        case Token::Type::RIGHT_BRACE:
            out << "RIGHT_BRACE " << token.mLexeme;
            break;
        case Token::Type::COMMA:
            out << "COMMA " << token.mLexeme;
            break;
        case Token::Type::DOT:
            out << "DOT " << token.mLexeme;
            break;
        case Token::Type::MINUS:
            out << "MINUS " << token.mLexeme;
            break;
        case Token::Type::PLUS:
            out << "PLUS " << token.mLexeme;
            break;
        case Token::Type::SEMICOLON:
            out << "SEMICOLON " << token.mLexeme;
            break;
        case Token::Type::SLASH:
            out << "SLASH " << token.mLexeme;
            break;
        case Token::Type::STAR:
            out << "STAR " << token.mLexeme;
            break;
        case Token::Type::BANG:
            out << "BANG " << token.mLexeme;
            break;
        case Token::Type::BANG_EQUAL:
            out << "BANG_EQUAL " << token.mLexeme;
            break;
        case Token::Type::EQUAL:
            out << "EQUAL " << token.mLexeme;
            break;
        case Token::Type::EQUAL_EQUAL:
            out << "EQUAL_EQUAL " << token.mLexeme;
            break;
        case Token::Type::GREATER:
            out << "GREATER " << token.mLexeme;
            break;
        case Token::Type::GREATER_EQUAL:
            out << "GREATER_EQUAL " << token.mLexeme;
            break;
        case Token::Type::LESS:
            out << "LESS " << token.mLexeme;
            break;
        case Token::Type::LESS_EQUAL:
            out << "LESS_EQUAL " << token.mLexeme;
            break;
        case Token::Type::IDENTIFIER:
            out << "IDENTIFIER " << token.mLexeme;
            break;
        case Token::Type::STRING:
            out << "STRING " << token.mLiteral.toString();
            break;
        case Token::Type::NUMBER:
            out << "NUMBER " << token.mLiteral.toString();
            break;
        case Token::Type::AND:
            out << "AND " << token.mLexeme;
            break;
        case Token::Type::CLASS:
            out << "CLASS " << token.mLexeme;
            break;
        case Token::Type::ELSE:
            out << "ELSE " << token.mLexeme;
            break;
        case Token::Type::FALSE:
            out << "FALSE " << token.mLexeme;
            break;
        case Token::Type::FUN:
            out << "FUN " << token.mLexeme;
            break;
        case Token::Type::FOR:
            out << "FOR " << token.mLexeme;
            break;
        case Token::Type::IF:
            out << "IF " << token.mLexeme;
            break;
        case Token::Type::NIL:
            out << "NIL " << token.mLexeme;
            break;
        case Token::Type::OR:
            out << "OR " << token.mLexeme;
            break;
        case Token::Type::PRINT:
            out << "PRINT " << token.mLexeme;
            break;
        case Token::Type::RETURN:
            out << "RETURN " << token.mLexeme;
            break;
        case Token::Type::SUPER:
            out << "SUPER " << token.mLexeme;
            break;
        case Token::Type::THIS:
            out << "THIS " << token.mLexeme;
            break;
        case Token::Type::TRUE:
            out << "TRUE " << token.mLexeme;
            break;
        case Token::Type::VAR:
            out << "VAR " << token.mLexeme;
            break;
        case Token::Type::WHILE:
            out << "WHILE " << token.mLexeme;
            break;
        case Token::Type::END_OF_FILE:
            out << "END_OF_FILE " << token.mLexeme;
            break;
    }

    return out;
}

Token::Type Token::getType() const {
    return mType;
}

Value Token::getLiteral() const {
    return mLiteral;
}

std::string Token::getLexeme() const {
    return mLexeme;
}

int Token::getLine() const {
    return mLine;
}

}  // namespace Lox

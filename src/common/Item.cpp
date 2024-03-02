#include <common/Item.hpp>

namespace Vought {

int Item::getLine() const {
    return mLine;
}

int Item::getColumn() const {
    return mColumn;
}

std::ostream& operator<<(std::ostream& out,
                         Item const& item) {
    item.print(out);

    return out;
}

void Error::print(std::ostream& out) const {
    out << "ERROR(location = " << mLine << ":" << mColumn
        << ", lexeme = {" << mLexeme << "}"
        << ", message = \"" << mMessage << "\")";
}

Token::Type Token::getType() const {
    return mType;
}

void Token::print(std::ostream& out) const {
    switch (mType) {
        case Type::FLOAT:
            out << "FLOAT(" << mValue.value().toString()
                << ")";
            break;
        case Type::RETURN_TYPE:
            out << "RETURN_TYPE";
            break;
        case Type::EXPONENT:
            out << "EXPONENT";
            break;
        case Type::COLON:
            out << "COLON";
            break;
        case Type::LEFT_PAREN:
            out << "LEFT_PAREN";
            break;
        case Type::RIGHT_PAREN:
            out << "RIGHT_PAREN";
            break;
        case Type::LEFT_BRACE:
            out << "LEFT_BRACE";
            break;
        case Type::RIGHT_BRACE:
            out << "RIGHT_BRACE";
            break;
        case Type::COMMA:
            out << "COMMA";
            break;
        case Type::DOT:
            out << "DOT";
            break;
        case Type::MINUS:
            out << "MINUS";
            break;
        case Type::PLUS:
            out << "PLUS";
            break;
        case Type::SEMICOLON:
            out << "SEMICOLON";
            break;
        case Type::SLASH:
            out << "SLASH";
            break;
        case Type::STAR:
            out << "STAR";
            break;
        case Type::BANG:
            out << "BANG";
            break;
        case Type::BANG_EQUAL:
            out << "BANG_EQUAL";
            break;
        case Type::EQUAL:
            out << "EQUAL";
            break;
        case Type::EQUAL_EQUAL:
            out << "EQUAL_EQUAL";
            break;
        case Type::GREATER:
            out << "GREATER";
            break;
        case Type::GREATER_EQUAL:
            out << "GREATER_EQUAL";
            break;
        case Type::LESS:
            out << "LESS";
            break;
        case Type::LESS_EQUAL:
            out << "LESS_EQUAL";
            break;
        case Type::IDENTIFIER:
            out << "IDENTIFIER("
                << mValue.value().toString() << ")"
                << " location = " << mLine << ":"
                << mColumn;
            break;
        case Type::STRING:
            out << "STRING(" << mValue.value().toString()
                << ")";
            break;
        case Type::INTEGER:
            out << "INTEGER(" << mValue.value().toString()
                << ")";
            break;
        case Type::AND:
            out << "AND";
            break;
        case Type::CLASS:
            out << "CLASS";
            break;
        case Type::ELSE:
            out << "ELSE";
            break;
        case Type::FALSE:
            out << "FALSE";
            break;
        case Type::FUN:
            out << "FUN";
            break;
        case Type::FOR:
            out << "FOR";
            break;
        case Type::IF:
            out << "IF";
            break;
        case Type::NIL:
            out << "NIL";
            break;
        case Type::OR:
            out << "OR";
            break;
        case Type::PRINT:
            out << "PRINT";
            break;
        case Type::RETURN:
            out << "RETURN";
            break;
        case Type::SUPER:
            out << "SUPER";
            break;
        case Type::THIS:
            out << "THIS";
            break;
        case Type::TRUE:
            out << "TRUE";
            break;
        case Type::VAR:
            out << "VAR";
            break;
        case Type::WHILE:
            out << "WHILE";
            break;
        case Type::WHITESPACE:
            out << "WHITESPACE"
                << " location = " << mLine << ":"
                << mColumn;
            break;
        case Type::END_OF_FILE:
            out << "END_OF_FILE";
            break;
    }
}

}  // namespace Vought

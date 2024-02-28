// std
#include <exception>
#include <utility>
#include <vector>

// lox
#include <errors/ScanningError.hpp>
#include <scanner/Scanner.hpp>

namespace Lox {

Scanner::Scanner(std::string source)
    : mSource(std::move(source)) {
}

void Scanner::scanTokens() {
    while (!isAtEnd()) {
        // We are at the beginning of the next lexeme
        mStart = mCurrent;
        scanToken();
    }

    mTokens.emplace_back(Token::Type::END_OF_FILE, "",
                         Value::createNil(), mLine);
}

std::vector<Token> Scanner::getTokens() {
    return std::move(mTokens);
}

bool Scanner::isAtEnd() {
    return mCurrent >= mSource.length();
}

void Scanner::scanToken() {
    char c = advance();

    switch (c) {
        case '(':
            addToken(Token::Type::LEFT_PAREN);
            break;
        case ')':
            addToken(Token::Type::RIGHT_PAREN);
            break;
        case '{':
            addToken(Token::Type::LEFT_BRACE);
            break;
        case '}':
            addToken(Token::Type::RIGHT_BRACE);
            break;
        case '.':
            addToken(Token::Type::DOT);
            break;
        case ',':
            addToken(Token::Type::COMMA);
            break;
        case ';':
            addToken(Token::Type::SEMICOLON);
            break;
        case '+':
            addToken(Token::Type::PLUS);
            break;
        case '-':
            addToken(Token::Type::MINUS);
            break;
        case '*':
            addToken(Token::Type::STAR);
            break;
        case '!':
            addToken(match('=') ? Token::Type::BANG_EQUAL
                                : Token::Type::BANG);
            break;
        case '=':
            addToken(match('=') ? Token::Type::EQUAL_EQUAL
                                : Token::Type::EQUAL);
            break;
        case '<':
            addToken(match('=') ? Token::Type::LESS_EQUAL
                                : Token::Type::LESS);
            break;
        case '>':
            addToken(match('=') ? Token::Type::GREATER_EQUAL
                                : Token::Type::GREATER);
            break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
            } else {
                addToken(Token::Type::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;
        case '\n':
            mLine++;
            break;
        case '"':
            string();
            break;
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                throw ScanningError(mLine,
                                    "unexpected character");
            }
            break;
    }
}

char Scanner::advance() {
    return mSource[mCurrent++];
}

void Scanner::addToken(Token::Type type) {
    switch (type) {
        case Token::Type::TRUE:
            addToken(type, Value::createBool(true));
            break;
        case Token::Type::FALSE:
            addToken(type, Value::createBool(false));
            break;
        default:
            addToken(type, Value::createNil());
    }
}

void Scanner::addToken(Token::Type type,
                       const Value& literal) {
    mTokens.emplace_back(
        type, mSource.substr(mStart, mCurrent - mStart),
        literal, mLine);
}

bool Scanner::match(char expected) {
    if (isAtEnd()) {
        return false;
    }

    if (mSource[mCurrent] != expected) {
        return false;
    }

    mCurrent++;

    return true;
}

char Scanner::peek() {
    if (isAtEnd()) {
        return '\0';
    }

    return mSource[mCurrent];
}

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            mLine++;
        }

        advance();
    }

    if (isAtEnd()) {
        throw ScanningError(mLine, "unterminated string");
    }

    // the closing "
    advance();

    // we can use substring constructor
    std::string value(mSource, mStart + 1,
                      mCurrent - 1 - mStart - 1);

    addToken(Token::Type::STRING,
             Value::createString(value));
}

void Scanner::number() {
    while (isDigit(peek())) {
        advance();
    }

    // look for fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // consume the "."
        advance();
        while (isDigit(peek())) {
            advance();
        }
    }

    // TODO: handle conversion errors which can be caused by
    // atof
    addToken(Token::Type::NUMBER,
             Value::createNumber(atof(
                 mSource.substr(mStart, mCurrent - mStart)
                     .c_str())));
}

char Scanner::peekNext() {
    if (mCurrent + 1 >= mSource.length()) {
        return '\0';
    }

    return mSource[mCurrent + 1];
}

void Scanner::identifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }

    std::string text =
        mSource.substr(mStart, mCurrent - mStart);

    try {
        addToken(keywords.at(text));
    } catch (std::exception& ex) {
        addToken(Token::Type::IDENTIFIER);
    }
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlphaNumeric(char c) {
    return isDigit(c) || isAlpha(c);
}

}  // namespace Lox

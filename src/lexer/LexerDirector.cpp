// vought
#include <lexer/LexerDirector.hpp>

namespace Vought {

enum Category {
    AND,
    BACKSLASH,
    BANG,
    COLON,
    COMMA,
    DIGIT,
    DOT,
    EQUAL,
    GREATER_THAN,
    LEFT_BRACE,
    LEFT_PAREN,
    LESS_THAN,
    LETTER,
    MINUS,
    NEWLINE,
    PIPE,
    PLUS,
    QUOTE,
    RIGHT_BRACE,
    RIGHT_PAREN,
    SEMICOLON,
    SLASH,
    STAR,
    UNDERSCORE,
    WHITESPACE,
};

Lexer LexerDirector::buildLexer(std::string const& source) {
    mBuilder.addSource(source)
        .addCategory(AND,
                     [](char c) -> bool {
                         return c == '&';
                     })
        .addCategory(BANG,
                     [](char c) -> bool {
                         return c == '!';
                     })
        .addCategory(COLON,
                     [](char c) -> bool {
                         return c == ':';
                     })
        .addCategory(COMMA,
                     [](char c) -> bool {
                         return c == ',';
                     })
        .addCategory(DIGIT,
                     [](char c) -> bool {
                         return isdigit(c);
                     })
        .addCategory(DOT,
                     [](char c) -> bool {
                         return c == '.';
                     })
        .addCategory(QUOTE,
                     [](char c) -> bool {
                         return c == '"';
                     })
        .addCategory(BACKSLASH,
                     [](char c) -> bool {
                         return c == '\\';
                     })
        .addCategory(EQUAL,
                     [](char c) -> bool {
                         return c == '=';
                     })
        .addCategory(GREATER_THAN,
                     [](char c) -> bool {
                         return c == '>';
                     })
        .addCategory(LEFT_BRACE,
                     [](char c) -> bool {
                         return c == '{';
                     })
        .addCategory(LEFT_PAREN,
                     [](char c) -> bool {
                         return c == '(';
                     })
        .addCategory(LESS_THAN,
                     [](char c) -> bool {
                         return c == '<';
                     })
        .addCategory(LETTER,
                     [](char c) -> bool {
                         return isalpha(c);
                     })
        .addCategory(MINUS,
                     [](char c) -> bool {
                         return c == '-';
                     })
        .addCategory(PIPE,
                     [](char c) -> bool {
                         return c == '|';
                     })
        .addCategory(PLUS,
                     [](char c) -> bool {
                         return c == '+';
                     })
        .addCategory(RIGHT_BRACE,
                     [](char c) -> bool {
                         return c == '}';
                     })
        .addCategory(RIGHT_PAREN,
                     [](char c) -> bool {
                         return c == ')';
                     })
        .addCategory(SEMICOLON,
                     [](char c) -> bool {
                         return c == ';';
                     })
        .addCategory(SLASH,
                     [](char c) -> bool {
                         return c == '/';
                     })
        .addCategory(STAR,
                     [](char c) -> bool {
                         return c == '*';
                     })
        .addCategory(UNDERSCORE,
                     [](char c) -> bool {
                         return c == '_';
                     })
        .addCategory(NEWLINE,
                     [](char c) -> bool {
                         return c == '\n';
                     })
        .addCategory(WHITESPACE, [](char c) -> bool {
            return c != '\n' && isspace(c);
        });

    // whitespace
    mBuilder.addTransition(0, {WHITESPACE, NEWLINE}, 1)
        .addTransition(1, {WHITESPACE, NEWLINE}, 1)
        .setStateAsFinal(1, Token::Type::WHITESPACE);

    // identifier
    mBuilder.addTransition(0, {LETTER, UNDERSCORE}, 2)
        .addTransition(2, {LETTER, DIGIT, UNDERSCORE}, 2)
        .setStateAsFinal(2, Token::Type::IDENTIFIER);

    // integers & floats
    mBuilder.addTransition(0, DIGIT, 3)
        .addTransition(3, DIGIT, 3)
        .setStateAsFinal(3, Token::Type::INTEGER)
        .addTransition(3, DOT, 4)
        .addTransition(4, DIGIT, 4)
        .setStateAsFinal(4, Token::Type::FLOAT);

    // puncutation "(", ")", "{", "}", ";", ",", ":"
    mBuilder.addTransition(0, LEFT_PAREN, 5)
        .addTransition(0, RIGHT_PAREN, 6)
        .addTransition(0, LEFT_BRACE, 7)
        .addTransition(0, RIGHT_BRACE, 8)
        .addTransition(0, SEMICOLON, 9)
        .addTransition(0, COMMA, 10)
        .addTransition(0, COLON, 11)
        .setStateAsFinal(5, Token::Type::LEFT_PAREN)
        .setStateAsFinal(6, Token::Type::RIGHT_PAREN)
        .setStateAsFinal(7, Token::Type::LEFT_BRACE)
        .setStateAsFinal(8, Token::Type::RIGHT_BRACE)
        .setStateAsFinal(9, Token::Type::SEMICOLON)
        .setStateAsFinal(10, Token::Type::COMMA)
        .setStateAsFinal(11, Token::Type::COLON);

    // "=", "=="
    mBuilder.addTransition(0, EQUAL, 12)
        .setStateAsFinal(12, Token::Type::EQUAL)
        .addTransition(12, EQUAL, 13)
        .setStateAsFinal(13, Token::Type::EQUAL_EQUAL);

    // "<", "<=" "<|>"
    mBuilder.addTransition(0, LESS_THAN, 14)
        .setStateAsFinal(14, Token::Type::LESS)
        .addTransition(14, EQUAL, 15)
        .setStateAsFinal(15, Token::Type::LESS_EQUAL)
        .addTransition(14, PIPE, 16)
        .addTransition(16, GREATER_THAN, 17)
        .setStateAsFinal(17, Token::Type::SWAP);

    // ">", ">="
    mBuilder.addTransition(0, GREATER_THAN, 18)
        .setStateAsFinal(18, Token::Type::GREATER)
        .addTransition(18, EQUAL, 19)
        .setStateAsFinal(19, Token::Type::GREATER_EQUAL);

    // "-", "->"
    mBuilder.addTransition(0, MINUS, 20)
        .setStateAsFinal(20, Token::Type::MINUS)
        .addTransition(20, GREATER_THAN, 21)
        .setStateAsFinal(21, Token::Type::RETURN_TYPE);

    // "*", "**"
    mBuilder.addTransition(0, STAR, 22)
        .setStateAsFinal(22, Token::Type::STAR)
        .addTransition(22, STAR, 23)
        .setStateAsFinal(23, Token::Type::EXPONENT);

    // "!", "!="
    mBuilder.addTransition(0, BANG, 24)
        .setStateAsFinal(24, Token::Type::BANG)
        .addTransition(24, EQUAL, 25)
        .setStateAsFinal(25, Token::Type::BANG_EQUAL);

    // "&&", "||"
    mBuilder.addTransition(0, AND, 26)
        .addTransition(26, AND, 27)
        .setStateAsFinal(27, Token::Type::AND)
        .addTransition(0, PIPE, 28)
        .addTransition(28, PIPE, 29)
        .setStateAsFinal(29, Token::Type::OR);

    // "+"
    mBuilder.addTransition(0, PLUS, 30)
        .setStateAsFinal(30, Token::Type::PLUS);

    // "/", "//", "/* ... */"
    mBuilder.addTransition(0, SLASH, 31)
        .setStateAsFinal(31, Token::Type::SLASH)
        .addTransition(31, SLASH, 32)
        .addComplementaryTransition(32, NEWLINE, 32)
        .setStateAsFinal(32, Token::Type::COMMENT)
        .addTransition(31, STAR, 33)
        .addComplementaryTransition(33, STAR, 33)
        .addTransition(33, STAR, 34)
        .addComplementaryTransition(34, SLASH, 33)
        .addTransition(34, SLASH, 35)
        .setStateAsFinal(34, Token::Type::COMMENT)
        .setStateAsFinal(35, Token::Type::COMMENT);

    mBuilder.addTransition(0, QUOTE, 36)
        .addComplementaryTransition(36, {BACKSLASH, QUOTE},
                                    36)
        .addTransition(36, BACKSLASH, 37)
        .addTransition(37, BACKSLASH, 36)
        .addTransition(37, QUOTE, 36)
        .addTransition(36, QUOTE, 38)
        .setStateAsFinal(38, Token::Type::STRING);

    mBuilder.setInitialState(0);

    return mBuilder.build();
}

}  // namespace Vought

// std
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

// vought
#include <lexer/Lexer.hpp>
#include <runner/Runner.hpp>

namespace Vought {

void Runner::run(std::string const& source) {
    LexerBuilder builder;

    builder.addSource(source)
        .addCategory(Category::AND,
                     [](char c) -> bool {
                         return c == '&';
                     })
        .addCategory(Category::BANG,
                     [](char c) -> bool {
                         return c == '!';
                     })
        .addCategory(Category::COLON,
                     [](char c) -> bool {
                         return c == ':';
                     })
        .addCategory(Category::COMMA,
                     [](char c) -> bool {
                         return c == ',';
                     })
        .addCategory(Category::DIGIT,
                     [](char c) -> bool {
                         return isdigit(c);
                     })
        .addCategory(Category::DOT,
                     [](char c) -> bool {
                         return c == '.';
                     })
        .addCategory(Category::EQUAL,
                     [](char c) -> bool {
                         return c == '=';
                     })
        .addCategory(Category::GREATER_THAN,
                     [](char c) -> bool {
                         return c == '>';
                     })
        .addCategory(Category::LEFT_BRACE,
                     [](char c) -> bool {
                         return c == '{';
                     })
        .addCategory(Category::LEFT_PAREN,
                     [](char c) -> bool {
                         return c == '(';
                     })
        .addCategory(Category::LESS_THAN,
                     [](char c) -> bool {
                         return c == '<';
                     })
        .addCategory(Category::LETTER,
                     [](char c) -> bool {
                         return isalpha(c);
                     })
        .addCategory(Category::MINUS,
                     [](char c) -> bool {
                         return c == '-';
                     })
        .addCategory(Category::PIPE,
                     [](char c) -> bool {
                         return c == '|';
                     })
        .addCategory(Category::PLUS,
                     [](char c) -> bool {
                         return c == '+';
                     })
        .addCategory(Category::RIGHT_BRACE,
                     [](char c) -> bool {
                         return c == '}';
                     })
        .addCategory(Category::RIGHT_PAREN,
                     [](char c) -> bool {
                         return c == ')';
                     })
        .addCategory(Category::SEMICOLON,
                     [](char c) -> bool {
                         return c == ';';
                     })
        .addCategory(Category::SLASH,
                     [](char c) -> bool {
                         return c == '/';
                     })
        .addCategory(Category::STAR,
                     [](char c) -> bool {
                         return c == '*';
                     })
        .addCategory(Category::UNDERSCORE,
                     [](char c) -> bool {
                         return c == '_';
                     })
        .addCategory(Category::NEWLINE,
                     [](char c) -> bool {
                         return c == '\n';
                     })
        .addCategory(Category::WHITESPACE,
                     [](char c) -> bool {
                         return c != '\n' && isspace(c);
                     });

    // whitespace
    builder.addTransition(0, {WHITESPACE, NEWLINE}, 1)
        .addTransition(1, {WHITESPACE, NEWLINE}, 1)
        .setStateAsFinal(1, Token::Type::WHITESPACE);

    // identifier
    builder.addTransition(0, LETTER, 2)
        .addTransition(0, UNDERSCORE, 2)
        .addTransition(2, LETTER, 2)
        .addTransition(2, DIGIT, 2)
        .addTransition(2, UNDERSCORE, 2)
        .setStateAsFinal(2, Token::Type::IDENTIFIER);

    // integers & floats
    builder.addTransition(0, DIGIT, 3)
        .addTransition(3, DIGIT, 3)
        .addTransition(3, DOT, 4)
        .addTransition(4, DIGIT, 4)
        .setStateAsFinal(3, Token::Type::INTEGER)
        .setStateAsFinal(4, Token::Type::FLOAT);

    // puncutation "(", ")", "{", "}", ";", ",", ":"
    builder.addTransition(0, LEFT_PAREN, 5)
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
    builder.addTransition(0, EQUAL, 12)
        .addTransition(12, EQUAL, 13)
        .setStateAsFinal(12, Token::Type::EQUAL)
        .setStateAsFinal(13, Token::Type::EQUAL_EQUAL);

    // "<", "<=" "<|>"
    builder.addTransition(0, LESS_THAN, 14)
        .addTransition(14, EQUAL, 15)
        .addTransition(14, PIPE, 16)
        .addTransition(16, GREATER_THAN, 17)
        .setStateAsFinal(14, Token::Type::LESS)
        .setStateAsFinal(15, Token::Type::LESS_EQUAL)
        .setStateAsFinal(17, Token::Type::SWAP);

    // ">", ">="
    builder.addTransition(0, GREATER_THAN, 18)
        .addTransition(18, EQUAL, 19)
        .setStateAsFinal(18, Token::Type::GREATER)
        .setStateAsFinal(19, Token::Type::GREATER_EQUAL);

    // "-", "->"
    builder.addTransition(0, MINUS, 20)
        .addTransition(20, GREATER_THAN, 21)
        .setStateAsFinal(20, Token::Type::MINUS)
        .setStateAsFinal(21, Token::Type::RETURN_TYPE);

    // "*", "**"
    builder.addTransition(0, STAR, 22)
        .addTransition(22, STAR, 23)
        .setStateAsFinal(22, Token::Type::STAR)
        .setStateAsFinal(23, Token::Type::EXPONENT);

    // "!", "!="
    builder.addTransition(0, BANG, 24)
        .addTransition(24, EQUAL, 25)
        .setStateAsFinal(24, Token::Type::BANG)
        .setStateAsFinal(25, Token::Type::BANG_EQUAL);

    // "&&", "||"
    builder.addTransition(0, AND, 26)
        .addTransition(26, AND, 27)
        .addTransition(0, PIPE, 28)
        .addTransition(28, PIPE, 29)
        .setStateAsFinal(27, Token::Type::AND)
        .setStateAsFinal(29, Token::Type::OR);

    // "+"
    builder.addTransition(0, PLUS, 30)
        .setStateAsFinal(30, Token::Type::PLUS);

    // "/"
    builder.addTransition(0, SLASH, 31)
        .addTransition(31, SLASH, 32)
        .addComplementaryTransition(32, NEWLINE, 32)
        .addTransition(32, NEWLINE, 33)
        .addTransition(31, STAR, 34)
        .addComplementaryTransition(34, STAR, 34)
        .addTransition(34, STAR, 35)
        .addComplementaryTransition(35, SLASH, 34)
        .addTransition(35, SLASH, 36)
        .setStateAsFinal(31, Token::Type::SLASH)
        .setStateAsFinal(33, Token::Type::COMMENT)
        .setStateAsFinal(34, Token::Type::COMMENT)
        .setStateAsFinal(36, Token::Type::COMMENT);

    builder.setInitialState(0);

    Lexer lexer = builder.build();

    lexer.getDFSA().print();

    for (;;) {
        std::optional<Token> token = lexer.nextToken();

        if (lexer.hasError()) {
            for (Error& error : lexer.getAllErrors()) {
                error.print(true);
            }

            break;
        } else {  // token should exist else crash
            token.value().print(true);

            if (token.value().getType() ==
                Token::Type::END_OF_FILE) {
                break;
            }
        }
    }
}

int Runner::runFile(std::string& path) {
    std::ifstream file(path);

    // make sure the file is opened correctly
    if (!file) {
        std::cerr << "vought: " << strerror(errno)
                  << std::endl;
        return EXIT_FAILURE;
    }

    // TODO: figure out when this fails and handle it
    // proplerly

    // get the length of the file
    file.seekg(0, std::ifstream::end);
    auto length = file.tellg();
    file.seekg(0, std::ifstream::beg);

    // load the source file into a string
    std::string source(length, '\0');
    file.read(source.data(), length);

    // close file
    file.close();

    // run the source file
    run(source);

    if (mHadScanningError || mHadParsingError) {
        return 65;
    } else if (mHadRuntimeError) {
        return 70;
    } else {
        return 0;
    }
}

int Runner::runPrompt() {
    std::string line;

    for (;;) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line.empty()) {
            break;
        }

        run(line);

        mHadScanningError = false;
        mHadParsingError = false;
    }

    return 0;
}

}  // namespace Vought

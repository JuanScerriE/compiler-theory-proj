// std
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

// lox
// #include <errors/RuntimeError.hpp>
// #include <errors/ScanningError.hpp>
// #include <parser/Parser.hpp>
#include <runner/Runner.hpp>
// #include <scanner/Scanner.hpp>
#include <lexer/Lexer.hpp>

namespace Vought {

// void Runner::handleError(ScanningError& error) {
//     std::cerr << "Error: " << error.what() << "\n[line "
//               << error.getLine() << "]" << std::endl;
//     mHadScanningError = true;
// }

// void Runner::handleError(ParsingError& error) {
//     if (error.getToken().getType() ==
//         Token::Type::END_OF_FILE) {
//         std::cerr << "Error at End Of File: "
//                   << error.what() << "\n[line "
//                   << error.getToken().getLine() << "]"
//                   << std::endl;
//     } else {
//         std::cerr << "Error at '"
//                   << error.getToken().getLexeme()
//                   << "': " << error.what() << "\n[line "
//                   << error.getToken().getLine() << "]"
//                   << std::endl;
//     }
//     mHadParsingError = true;
// }

// void Runner::handleError(RuntimeError& error) {
//     std::cerr << "Error: " << error.what() << "\n[line "
//               << error.getToken().getLine() << "]"
//               << std::endl;
//     mHadRuntimeError = true;
// }

// void Runner::run(std::string const& source) {
//     Scanner scanner(source);
//
//     try {
//         scanner.scanTokens();
//     } catch (ScanningError& error) {
//         handleError(error);
//     }
//
//     if (mHadScanningError)
//         return;
//
//     std::vector<Token> tokens = scanner.getTokens();
//
//     // print the tokens
//     for (Token const& token : tokens) {
//         std::cout << token << std::endl;
//     }
//
//     Parser parser(tokens);
//
//     try {
//         parser.parse();
//     } catch (ParsingError& error) {
//         handleError(error);
//     }
//
//     std::unique_ptr<Program> program = parser.getAST();
//
//     if (mHadParsingError)
//         return;
//
//     // print the ast
//     mPrinter.print(program);
//
//     // interpret the ast
//     // mInterpreter.interpret(program);
// }
//
// int Runner::runFile(std::string& path) {
//     std::ifstream file(path);
//
//     // make sure the file is opened correctly
//     if (!file) {
//         std::cerr << "lox: " << strerror(errno)
//                   << std::endl;
//         return EXIT_FAILURE;
//     }
//
//     // TODO: figure out when this fails and handle it
//     // proplerly
//
//     // get the length of the file
//     file.seekg(0, std::ifstream::end);
//     auto length = file.tellg();
//     file.seekg(0, std::ifstream::beg);
//
//     // load the source file into a string
//     std::string source(length, '\0');
//     file.read(source.data(), length);
//
//     // close file
//     file.close();
//
//     // run the source file
//     run(source);
//
//     if (mHadScanningError || mHadParsingError) {
//         return 65;
//     } else if (mHadRuntimeError) {
//         return 70;
//     } else {
//         return 0;
//     }
// }
//
// int Runner::runPrompt() {
//     std::string line;
//
//     for (;;) {
//         std::cout << "> ";
//         std::getline(std::cin, line);
//
//         if (line.empty()) {
//             break;
//         }
//
//         run(line);
//
//         mHadScanningError = false;
//         mHadParsingError = false;
//     }
//
//     return 0;
// }

void Runner::run(std::string const& source) {
    Vought::Lexer lexer(source, DFSA(32, CATEGORY_SIZE));

    // whitespace
    lexer
        .createTransitionAsFinal(START_STATE, WHITESPACE, 1,
                                 Token::Type::WHITESPACE)
        .createTransition(1, WHITESPACE, 1);

    // identifier
    lexer
        .createTransitionAsFinal(START_STATE, LETTER, 2,
                                 Token::Type::IDENTIFIER)
        .createTransition(START_STATE, UNDERSCORE, 2)
        .createTransition(2, LETTER, 2)
        .createTransition(2, DIGIT, 2)
        .createTransition(2, UNDERSCORE, 2);

    // integers & floats
    lexer
        .createTransitionAsFinal(START_STATE, DIGIT, 3,
                                 Token::Type::INTEGER)
        .createTransition(3, DIGIT, 3)
        .createTransitionAsFinal(3, DOT, 4,
                                 Token::Type::FLOAT)
        .createTransition(4, DIGIT, 4);

    // puncutation "(", ")", "{", "}", ";", ",", ":"
    lexer
        .createTransitionAsFinal(START_STATE, LEFT_PAREN, 5,
                                 Token::Type::LEFT_PAREN)
        .createTransitionAsFinal(START_STATE, RIGHT_PAREN,
                                 6,
                                 Token::Type::RIGHT_PAREN)
        .createTransitionAsFinal(START_STATE, LEFT_BRACE, 7,
                                 Token::Type::LEFT_BRACE)
        .createTransitionAsFinal(START_STATE, RIGHT_BRACE,
                                 8,
                                 Token::Type::RIGHT_BRACE)
        .createTransitionAsFinal(START_STATE, SEMICOLON, 9,
                                 Token::Type::SEMICOLON)
        .createTransitionAsFinal(START_STATE, COMMA, 10,
                                 Token::Type::COMMA)
        .createTransitionAsFinal(START_STATE, COLON, 11,
                                 Token::Type::COLON);

    // "=", "=="
    lexer
        .createTransitionAsFinal(START_STATE, EQUAL, 12,
                                 Token::Type::EQUAL)
        .createTransitionAsFinal(12, EQUAL, 13,
                                 Token::Type::EQUAL_EQUAL);

    // "<", "<=" "<|>"
    lexer
        .createTransitionAsFinal(START_STATE, LESS_THAN, 14,
                                 Token::Type::LESS)
        .createTransitionAsFinal(14, EQUAL, 15,
                                 Token::Type::LESS_EQUAL)
        .createTransition(14, PIPE, 30)
        .createTransitionAsFinal(30, GREATER_THAN, 31,
                                 Token::Type::SWAP);

    // ">", ">="
    lexer
        .createTransitionAsFinal(START_STATE, GREATER_THAN,
                                 16, Token::Type::GREATER)
        .createTransitionAsFinal(
            16, EQUAL, 17, Token::Type::GREATER_EQUAL);

    // "-", "->"
    lexer
        .createTransitionAsFinal(START_STATE, MINUS, 18,
                                 Token::Type::MINUS)
        .createTransitionAsFinal(18, GREATER_THAN, 19,
                                 Token::Type::RETURN_TYPE);

    // "*", "**"
    lexer
        .createTransitionAsFinal(START_STATE, STAR, 20,
                                 Token::Type::STAR)
        .createTransitionAsFinal(20, STAR, 21,
                                 Token::Type::EXPONENT);

    // "!", "!="
    lexer
        .createTransitionAsFinal(START_STATE, BANG, 22,
                                 Token::Type::BANG)
        .createTransitionAsFinal(22, EQUAL, 23,
                                 Token::Type::BANG_EQUAL);

    // "&&", "||"
    lexer.createTransition(START_STATE, AND, 24)
        .createTransitionAsFinal(24, AND, 25,
                                 Token::Type::AND)
        .createTransition(START_STATE, PIPE, 26)
        .createTransitionAsFinal(26, PIPE, 27,
                                 Token::Type::OR);

    // "+"
    lexer.createTransitionAsFinal(START_STATE, PLUS, 28,
                                  Token::Type::PLUS);

    // division
    lexer.createTransitionAsFinal(START_STATE, SLASH, 29,
                                  Token::Type::SLASH);

    for (;;) {
        std::optional<Token> token = lexer.nextToken();

        if (lexer.hasError()) {
            for (Error& error : lexer.getAllErrors()) {
                error.print(true);
            }

            break;
        } else {  // token should exist else crash
            if (token.value().getType() !=
                Token::Type::WHITESPACE)
                token.value().print(true);

            if (token.value().getType() ==
                Token::Type::END_OF_FILE) {
                break;
            }
        }
    }

    std::cout << std::endl;
}

// Parser parser(tokens);
//
// try {
//     parser.parse();
// } catch (ParsingError& error) {
//     handleError(error);
// }
//
// std::unique_ptr<Program> program = parser.getAST();
//
// if (mHadParsingError)
//     return;
//
// // print the ast
// mPrinter.print(program);

// interpret the ast
// mInterpreter.interpret(program);

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

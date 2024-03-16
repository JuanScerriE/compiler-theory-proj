// std
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

// vought
#include <lexer/LexerDirector.hpp>
#include <runner/Runner.hpp>

namespace Vought {

void Runner::run(std::string const& source) {
    LexerDirector director;

    Lexer lexer = director.buildLexer(source);

    lexer.getDFSA().print();

    for (;;) {
        std::optional<Token> token = lexer.nextToken();

        if (lexer.hasError()) {
            for (Error& error : lexer.getAllErrors()) {
                error.print(true);
            }

            break;
        } else {  // token should exist else crash
            if (!(token->getType() == Token::Type::WHITESPACE || token->getType() == Token::Type::COMMENT))
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

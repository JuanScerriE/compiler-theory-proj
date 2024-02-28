// std
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

// lox
#include <errors/RuntimeError.hpp>
#include <errors/ScanningError.hpp>
#include <parser/Parser.hpp>
#include <runner/Runner.hpp>
#include <scanner/Scanner.hpp>

namespace Lox {

void Runner::handleError(ScanningError& error) {
    std::cerr << "Error: " << error.what() << "\n[line "
              << error.getLine() << "]" << std::endl;
    mHadScanningError = true;
}

void Runner::handleError(ParsingError& error) {
    if (error.getToken().getType() ==
        Token::Type::END_OF_FILE) {
        std::cerr << "Error at End Of File: "
                  << error.what() << "\n[line "
                  << error.getToken().getLine() << "]"
                  << std::endl;
    } else {
        std::cerr << "Error at '"
                  << error.getToken().getLexeme()
                  << "': " << error.what() << "\n[line "
                  << error.getToken().getLine() << "]"
                  << std::endl;
    }
    mHadParsingError = true;
}

void Runner::handleError(RuntimeError& error) {
    std::cerr << "Error: " << error.what() << "\n[line "
              << error.getToken().getLine() << "]"
              << std::endl;
    mHadRuntimeError = true;
}

void Runner::run(std::string const& source) {
    Scanner scanner(source);

    try {
        scanner.scanTokens();
    } catch (ScanningError& error) {
        handleError(error);
    }

    if (mHadScanningError)
        return;

    std::vector<Token> tokens = scanner.getTokens();

    // print the tokens
    for (Token const& token : tokens) {
        std::cout << token << std::endl;
    }

    Parser parser(tokens);

    try {
        parser.parse();
    } catch (ParsingError& error) {
        handleError(error);
    }

    std::unique_ptr<Program> program = parser.getAST();

    if (mHadParsingError)
        return;

    // print the ast
    mPrinter.print(program);

    // interpret the ast
    // mInterpreter.interpret(program);
}

int Runner::runFile(std::string& path) {
    std::ifstream file(path);

    // make sure the file is opened correctly
    if (!file) {
        std::cerr << "lox: " << strerror(errno)
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

}  // namespace Lox

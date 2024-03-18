// std
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

// vought
#include <common/Token.hpp>
#include <lexer/LexerDirector.hpp>
#include <parser/Parser.hpp>
#include <parser/PrinterVisitor.hpp>
#include <runner/Runner.hpp>

// fmt
#include <fmt/core.h>

namespace Vought {

Runner::Runner(bool dfsaDbg, bool lexerDbg, bool parserDbg)
    : mDfsaDbg(dfsaDbg),
      mLexerDbg(lexerDbg),
      mParserDbg(parserDbg) {
}

void Runner::run(std::string const& source) {
    LexerDirector director;

    Lexer lexer = director.buildLexer(source);

    if (mDfsaDbg) {
        fmt::println("DFSA DEBUG PRINT");

        lexer.getDFSA().print();

        fmt::print("\n");
    }

    if (mLexerDbg) {
        fmt::println("LEXER DEBUG PRINT");

        Lexer dbgLexer = director.buildLexer(source);

        for (;;) {
            std::optional<Token> token =
                dbgLexer.nextToken();

            if (lexer.hasError()) {
                for (Error& error :
                     dbgLexer.getAllErrors()) {
                    fmt::println("{}",
                                 error.toString(true));
                }

                break;
            } else {  // token should exist else crash
                fmt::println("{}",
                             token.value().toString(true));

                if (token.value().getType() ==
                    Token::Type::END_OF_FILE) {
                    break;
                }
            }
        }

        fmt::print("\n");
    }

    Parser parser(lexer);

    parser.parse();

    std::unique_ptr<Program> ast = parser.getAST();

    if (mParserDbg) {
        fmt::println("PARSER DEBUG PRINT");

        PrinterVisitor printer;

        ast->accept(&printer);

        fmt::print("\n");
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

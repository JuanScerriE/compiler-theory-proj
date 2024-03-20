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

#include "analysis/AnalysisVisitor.hpp"

// fmt
#include <fmt/core.h>

namespace Vought {

Runner::Runner(bool dfsaDbg, bool lexerDbg, bool parserDbg)
    : mDfsaDbg(dfsaDbg),
      mLexerDbg(lexerDbg),
      mParserDbg(parserDbg),
      mLexer(LexerDirector::buildLexer()),
      mParser(Parser(mLexer)) {
}

void Runner::run(std::string const& source) {
    if (mDfsaDbg) {
        fmt::println("DFSA DEBUG PRINT");

        mLexer.getDFSA().print();

        fmt::print("\n");
    }

    if (mLexerDbg) {
        fmt::println("LEXER DEBUG PRINT");

        mLexer.addSource(source);

        for (;;) {
            std::optional<Token> token = mLexer.nextToken();

            if (mLexer.hasError()) {
                mHadLexingError = true;

                return;
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

    mLexer.addSource(source);

    mParser.reset();

    try {
        mParser.parse();
    } catch (LexerError&) {
        mHadLexingError = true;

        return;
    }

    if (mParser.hasError()) {
        mHadParsingError = true;

        return;
    }

    std::unique_ptr<Program> ast = mParser.getAST();

    if (mParserDbg) {
        fmt::println("PARSER DEBUG PRINT");

        PrinterVisitor printer;

        ast->accept(&printer);

        fmt::print("\n");
    }

    ast->accept(&mAnalyser);
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

    if (mHadLexingError || mHadParsingError)
        return 65;

    return 0;
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

        mHadLexingError = false;
        mHadParsingError = false;
    }

    return 0;
}

}  // namespace Vought

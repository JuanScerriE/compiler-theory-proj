// std
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

// vought
#include <lexer/LexerDirector.hpp>
#include <parser/Parser.hpp>
#include <parser/PrinterVisitor.hpp>
#include <runner/Runner.hpp>

namespace Vought {

void Runner::run(std::string const& source) {
    LexerDirector director;

    Lexer lexer = director.buildLexer(source);

    lexer.getDFSA().print();

    Parser parser(lexer);

    parser.parse();

    std::unique_ptr<Program> ast = parser.getAST();

    std::unique_ptr<PrinterVisitor> printer = std::make_unique<PrinterVisitor>();

    ast->accept(printer.get());
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

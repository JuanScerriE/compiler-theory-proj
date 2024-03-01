#pragma once

// std
#include <string>

// lox
// #include <common/Token.hpp>
// #include <errors/ParsingError.hpp>
// #include <errors/RuntimeError.hpp>
// #include <errors/ScanningError.hpp>
// #include <evaluator/Interpreter.hpp>
// #include <printer/ASTPrinter.hpp>

namespace Vought {

class Runner {
   public:
    int runFile(std::string& path);
    int runPrompt();

   private:
    // TODO: the funny thing here is that we are
    // actually going to change the error system
    // so that we can report all syntax errors upto
    // some maximum. Just so we give the user as much insite
    // into the possible lexical errors he has made.

    // void handleError(ScanningError& error);
    // void handleError(ParsingError& error);
    // void handleError(RuntimeError& error);

    void run(std::string const& source);

    bool mHadScanningError = false;
    bool mHadParsingError = false;
    bool mHadRuntimeError = false;

    // Interpreter mInterpreter;
    // ASTPrinter mPrinter;
};

}  // namespace Vought

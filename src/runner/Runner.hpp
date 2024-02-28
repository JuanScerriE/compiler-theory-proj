#pragma once

// std
#include <string>

// lox
#include <common/Token.hpp>
#include <errors/ParsingError.hpp>
#include <errors/RuntimeError.hpp>
#include <errors/ScanningError.hpp>
#include <evaluator/Interpreter.hpp>
#include <printer/ASTPrinter.hpp>

namespace Lox {

class Runner {
   public:
    int runFile(std::string& path);
    int runPrompt();

   private:
    void handleError(ScanningError& error);
    void handleError(ParsingError& error);
    void handleError(RuntimeError& error);

    void run(std::string const& source);

    bool mHadScanningError = false;
    bool mHadParsingError = false;
    bool mHadRuntimeError = false;

    Interpreter mInterpreter;
    ASTPrinter mPrinter;
};

}  // namespace Lox

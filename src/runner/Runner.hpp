#pragma once

// std
#include <string>

// vought
#include <common/Token.hpp>

namespace Vought {

class Runner {
   public:
    Runner(bool dfsaDbg, bool lexerDbg, bool parserDbg);

    int runFile(std::string& path);
    int runPrompt();

   private:
    void report(int line, int column, Token const& token,
                std::string const& message) const;

    void run(std::string const& source);

    bool mHadScanningError = false;
    bool mHadParsingError = false;
    bool mHadRuntimeError = false;

    bool mDfsaDbg = false;
    bool mLexerDbg = false;
    bool mParserDbg = false;
};

}  // namespace Vought

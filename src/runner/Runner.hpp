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

    static void toggleLexingError() noexcept;
    static void toggleParsingError() noexcept;
    static bool hasLexingError() noexcept;
    static bool hasParsingError() noexcept;

    // static void report(int line, int column, Token const&
    // token,
    //             std::string const& message);
   private:
    void run(std::string const& source);

    static bool mHadLexingError;
    static bool mHadParsingError;

    bool mDfsaDbg = false;
    bool mLexerDbg = false;
    bool mParserDbg = false;
};

}  // namespace Vought

#pragma once

// std
#include <string>

// parl
#include <common/Token.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>
#include <analysis/AnalysisVisitor.hpp>

namespace PArL {

class Runner {
   public:
    Runner(bool dfsaDbg, bool lexerDbg, bool parserDbg);

    int runFile(std::string& path);
    int runPrompt();

   private:
    void run(std::string const& source);

    void debugDfsa();
    void debugLexeing();
    void debugParsing();
    void debugAnalysis();

    bool mHadLexingError = false;
    bool mHadParsingError = false;
    bool mHadAnalysisError = false;

    bool mDfsaDbg = false;
    bool mLexerDbg = false;
    bool mParserDbg = false;

    Lexer mLexer;
    Parser mParser;
    AnalysisVisitor mAnalyser{};
};

}  // namespace PArL

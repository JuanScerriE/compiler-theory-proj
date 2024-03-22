#pragma once

// vought
#include <common/Token.hpp>
#include <lexer/LexerBuilder.hpp>

namespace PArL {

class LexerDirector {
   public:
    static Lexer buildLexer();
};

}  // namespace PArL

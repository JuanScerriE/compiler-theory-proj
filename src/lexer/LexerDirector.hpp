#pragma once

// vought
#include <common/Token.hpp>
#include <lexer/LexerBuilder.hpp>

namespace Vought {

class LexerDirector {
   public:
    static Lexer buildLexer();
};

}  // namespace Vought

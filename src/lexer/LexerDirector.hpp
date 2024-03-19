#pragma once

// vought
#include <common/Token.hpp>
#include <lexer/LexerBuilder.hpp>

namespace Vought {

class LexerDirector {
   public:
    Lexer buildLexer();

   private:
    LexerBuilder mBuilder{};
};

}  // namespace Vought

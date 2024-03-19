#pragma once

// vought
#include <common/Token.hpp>
#include <lexer/LexerBuilder.hpp>

namespace Vought {

class LexerDirector {
   public:
    Lexer buildLexer(std::string const& source);

   private:
    LexerBuilder mBuilder{};
};

}  // namespace Vought

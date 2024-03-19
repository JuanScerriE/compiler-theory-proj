#pragma once

// vought
#include <common/Value.hpp>

namespace Vought {

class Item {
   public:
    Item(int line, int column, std::string const& lexeme);

    [[nodiscard]] int getLine() const;
    [[nodiscard]] int getColumn() const;
    [[nodiscard]] std::string getLexeme() const;

    virtual std::string toString(
        bool withLocation) const = 0;

    virtual ~Item() = default;

   protected:
    int mLine;
    int mColumn;
    std::string mLexeme;
};

}  // namespace Vought

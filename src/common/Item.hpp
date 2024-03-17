#pragma once

// vought
#include <common/Value.hpp>

namespace Vought {

class Item {
   public:
    Item(int line, int column, std::string const& lexeme)
        : mLine(line), mColumn(column), mLexeme(lexeme) {
    }

    Item(Item const& other) {
        mLine = other.mLine;
        mColumn = other.mColumn;
        mLexeme = other.mLexeme;
    }

    [[nodiscard]] int getLine() const;
    [[nodiscard]] int getColumn() const;
    [[nodiscard]] std::string getLexeme() const;

    virtual std::string toString(bool withLocation) const = 0;

   protected:
    int mLine;
    int mColumn;
    std::string mLexeme;
};

}  // namespace Vought

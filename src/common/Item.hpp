#pragma once

// vought
#include <common/Value.hpp>

namespace Vought {

class Item {
   public:
    Item(int line, int column)
        : mLine(line), mColumn(column) {
    }

    Item(Item const& other) {
        mLine = other.mLine;
        mColumn = other.mColumn;
    }

    [[nodiscard]] int getLine() const;
    [[nodiscard]] int getColumn() const;

    virtual void print(bool withLocation) const = 0;

   protected:
    int mLine;
    int mColumn;
};

}  // namespace Vought

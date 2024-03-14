#pragma once

// vought
#include <common/Item.hpp>

namespace Vought {

class Error : public Item {
   public:
    Error(int line, int column, std::string lexeme,
          std::string message)
        : Item(line, column, lexeme),
          mMessage(message) {
    }

    Error(Error const& other)
        : Item(other),
          mMessage(other.mMessage) {
    }

    void print(bool withLocation) const override;

   private:
    std::string mMessage;
};

}  // namespace Vought

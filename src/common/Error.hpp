#pragma once

// vought
#include <common/Item.hpp>

namespace Vought {

class Error : public Item {
   public:
    Error(int line, int column, std::string const& lexeme);

    std::string toString(bool withLocation) const override;
};

}  // namespace Vought

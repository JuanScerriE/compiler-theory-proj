// vought
#include <common/Item.hpp>

namespace Vought {

int Item::getLine() const {
    return mLine;
}

int Item::getColumn() const {
    return mColumn;
}

std::string Item::getLexeme() const {
    return mLexeme;
}

}  // namespace Vought

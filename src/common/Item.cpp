// vought
#include <common/Item.hpp>

namespace Vought {

Item::Item(int line, int column, std::string const& lexeme)
    : mLine(line), mColumn(column), mLexeme(lexeme) {
}

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

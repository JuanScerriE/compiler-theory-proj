// fmt
#include <fmt/core.h>

// vought
#include <common/Error.hpp>

namespace PArL {

Error::Error(int line, int column,
             std::string const& lexeme)
    : Item(line, column, lexeme) {
}

std::string Error::toString(bool withLocation) const {
    if (withLocation)
        return fmt::format("{}:{} ERROR(lexeme = \"{}\")",
                           mLine, mColumn, mLexeme);
    else
        return fmt::format("ERROR(lexeme = \"{}\")",
                           mLexeme);
}

}  // namespace PArL

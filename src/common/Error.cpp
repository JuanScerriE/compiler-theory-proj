// fmt
#include <fmt/core.h>

// vought
#include <common/Error.hpp>

namespace Vought {

std::string Error::toString(bool withLocation) const {
    if (withLocation)
        return fmt::format(
            "{}:{} ERROR(lexeme = \"{}\", message "
            "= \"{}\")",
            mLine, mColumn, mLexeme, mMessage);
    else
        return fmt::format(
            "ERROR(lexeme = \"{}\", message "
            "= \"{}\")",
            mLexeme, mMessage);
}

}  // namespace Vought

// fmt
#include <fmt/core.h>

// vought
#include <common/Error.hpp>

namespace Vought {

void Error::print(bool withLocation) const {
    if (withLocation)
        fmt::print("{}:{} ", mLine, mColumn);

    fmt::println(
        "ERROR(lexeme = \"{}\", message "
        "= \"{}\")",
        mLexeme, mMessage);
}

}  // namespace Vought

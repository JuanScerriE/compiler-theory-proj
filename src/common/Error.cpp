// fmt
#include <fmt/core.h>

// vought
#include <common/Error.hpp>

namespace Vought {

Error::Error(int line, int column, std::string lexeme,
             std::string message)
    : Item(line, column, lexeme), mMessage(message) {
}

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

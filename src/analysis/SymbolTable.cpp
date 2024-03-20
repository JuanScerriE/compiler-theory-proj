// vought
#include <analysis/SymbolTable.hpp>
#include <common/Abort.hpp>

namespace Vought {

void SymbolTable::addIdentifier(
    std::string const& identifier, Signature signature) {
    if (mMap.count(identifier) > 0) {
        throw RepeatSymbolException();
    }

    // "cannot repeat identifiers in same scope");

    mMap.insert({identifier, signature});
}

std::optional<Signature> SymbolTable::findIdenfitier(
    std::string const& identifier) const {
    if (mMap.count(identifier) > 0)
        return mMap.at(identifier);

    return {};
}

}  // namespace Vought

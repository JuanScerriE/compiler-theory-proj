// vought
#include <analysis/SymbolTable.hpp>
#include <common/Assert.hpp>

namespace Vought {

void SymbolTable::addIdentifier(
    std::string const& identifier, Signature signature) {
    // TODO: remove the abort since this is user
    // controlled
    ABORTIF(mMap.count(identifier) > 0,
            "cannot repeat identifiers in same scope");

    mMap[identifier] = signature;
}

std::optional<Signature> SymbolTable::findIdenfitier(
    std::string const& identifier) const {
    if (mMap.count(identifier) > 0)
        return mMap.at(identifier);

    return {};
}

}  // namespace Vought

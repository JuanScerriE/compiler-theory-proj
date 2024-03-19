// vought
#include <analysis/Signature.hpp>
#include <analysis/SymbolStack.hpp>
#include <common/Assert.hpp>

namespace Vought {

void SymbolStack::addIdentifier(
    std::string const& identifier, Signature signature) {
    currentScope().addIdentifier(identifier, signature);
}

std::optional<Signature> SymbolStack::findIdentifier(
    std::string const& identifier) const {
    for (auto const& table : mStack) {
        std::optional<Signature> signature =
            table.findIdenfitier(identifier);

        if (signature.has_value())
            return signature;
    }

    return {};
}

void SymbolStack::push() {
    mStack.emplace_front();
}

void SymbolStack::pop() {
    mStack.pop_front();
}

SymbolTable& SymbolStack::currentScope() {
    return mStack.front();
}

bool SymbolStack::isCurrentScopeGlobal() const {
    return mStack.size() <= 1;
}

}  // namespace Vought

// parl
#include <analysis/SymbolStack.hpp>

namespace PArL {

SymbolStack& SymbolStack::pushScope() {
    SymbolTable& scope = mStack.front();

    mStack.emplace_front();

    mStack.front().setEnclosing(&scope);

    return *this;
}

SymbolStack& SymbolStack::popScope() {
    mStack.pop_front();

    return *this;
}

SymbolStack& SymbolStack::setType(SymbolTable::Type type) {
    mStack.front().setType(type);

    return *this;
}

SymbolStack& SymbolStack::setName(std::string const& name) {
    mStack.front().setName(name);

    return *this;
}

SymbolTable* SymbolStack::currentScope() {
    return &mStack.front();
}

bool SymbolStack::isCurrentScopeGlobal() const {
    return mStack.front().isGlobalScope();
}

}  // namespace PArL

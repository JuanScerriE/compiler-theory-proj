// parl
#include <analysis/SymbolStack.hpp>

namespace PArL {

// void SymbolStack::addIdentifier(
//     std::string const& identifier,
//     Signature const& signature) {
//     currentScope().addIdentifier(identifier, signature);
// }
//
// std::optional<Signature> SymbolStack::findIdentifier(
//     std::string const& identifier) const {
//     SymbolTable const& currentScope = mStack.front();
//
//     return currentScope.findIdenfitier(identifier);
// }

// std::optional<std::pair<std::string, Signature>>
// SymbolStack::getEnclosingFunction() const {
//     auto start = ++(mStack.begin());
//     auto end = mStack.end();
//
//     for (; start != end; start++) {
//         for (auto& [ident, sig] : start->mMap) {
//             if (sig.is<FunctionSignature>()) {
//                 std::pair<std::string, Signature> found{
//                     ident, sig};
//
//                 return found;
//             }
//         }
//     }
//
//     return {};
// }

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

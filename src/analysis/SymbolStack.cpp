// vought
#include <analysis/Signature.hpp>
#include <analysis/SymbolStack.hpp>
#include <common/Abort.hpp>

#include "analysis/SymbolTable.hpp"

namespace PArL {

void SymbolStack::addIdentifier(
    std::string const& identifier, Signature signature) {
    currentScope().addIdentifier(identifier, signature);
}

std::optional<Signature> SymbolStack::findIdentifier(
    std::string const& identifier) const {
    SymbolTable const& currentScope = mStack.front();

    return currentScope.findIdenfitier(identifier);
}

std::optional<std::pair<std::string, Signature>>
SymbolStack::getEnclosingFunction() const {
    auto start = ++(mStack.begin());
    auto end = mStack.end();

    for (; start != end; start++) {
        for (auto& [ident, sig] : start->mMap) {
            if (sig.isFunctionSig()) {
                std::pair<std::string, Signature> found{
                    ident, sig};

                return found;
            }
        }
    }

    return {};
}

void SymbolStack::pushScope() {
    SymbolTable& currentScope = mStack.front();

    std::vector<Rule> enclosingInsertRules =
        currentScope.getInsertRules();

    mStack.emplace_front();

    mStack.front().setEnclosing(&currentScope);
    mStack.front().addInsertRule(enclosingInsertRules);
}

void SymbolStack::pushScope(
    std::initializer_list<Rule> insertRules,
    std::initializer_list<Rule> searchRules) {
    SymbolTable& currentScope = mStack.front();

    std::vector<Rule> enclosingInsertRules =
        currentScope.getInsertRules();

    mStack.emplace_front();

    mStack.front().setEnclosing(&currentScope);
    mStack.front().addInsertRule(enclosingInsertRules);

    mStack.front().addInsertRule(insertRules);
    mStack.front().addSearchRule(searchRules);
}

void SymbolStack::popScope() {
    mStack.pop_front();
}

SymbolTable& SymbolStack::currentScope() {
    return mStack.front();
}

bool SymbolStack::isCurrentScopeGlobal() const {
    return mStack.front().isGlobalScope();
}

}  // namespace PArL

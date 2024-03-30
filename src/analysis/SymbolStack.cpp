// parl
#include <analysis/SymbolStack.hpp>

namespace PArL {

SymbolStack::SymbolStack() {
    mGlobal = std::make_unique<Environment>();

    mCurrent = mGlobal.get();
}

SymbolStack& SymbolStack::pushScope() {
    auto& ref = mCurrent->children().emplace_back(
        std::make_unique<Environment>()
    );

    ref->setEnclosing(mCurrent);

    mCurrent = ref.get();

    return *this;
}

SymbolStack& SymbolStack::popScope() {
    mCurrent = mCurrent->getEnclosing();

    return *this;
}

SymbolStack& SymbolStack::setType(Environment::Type type) {
    mCurrent->setType(type);

    return *this;
}

SymbolStack& SymbolStack::setName(std::string const& name) {
    mCurrent->setName(name);

    return *this;
}

Environment* SymbolStack::currentScope() {
    return mCurrent;
}

std::unique_ptr<Environment> SymbolStack::getGlobal() {
    return std::move(mGlobal);
}

bool SymbolStack::isCurrentScopeGlobal() const {
    return mCurrent->isGlobalScope();
}

}  // namespace PArL

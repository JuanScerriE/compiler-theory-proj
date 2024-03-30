#pragma once

// parl
#include <backend/Environment.hpp>
#include <backend/Symbol.hpp>

// std
#include <list>

namespace PArL {

class SymbolStack {
   public:
    SymbolStack();

    SymbolStack& pushScope();
    SymbolStack& popScope();

    SymbolStack& setType(Environment::Type type);
    SymbolStack& setName(std::string const& name);

    Environment* currentScope();

    [[nodiscard]] std::unique_ptr<Environment> getGlobal();

    [[nodiscard]] bool isCurrentScopeGlobal() const;

   private:
    std::unique_ptr<Environment> mGlobal{};
    Environment* mCurrent{};
};

}  // namespace PArL

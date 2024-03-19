#pragma once

// vought
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>

// std
#include <list>

namespace Vought {
class SymbolStack {
   public:
    void addIdentifier(std::string const& identifier,
                       Signature signature);

    std::optional<Signature> findIdentifier(
        std::string const& identifier) const;

    void push();
    void pop();

    SymbolTable& currentScope();

    bool isCurrentScopeGlobal() const;

   private:
    std::list<SymbolTable> mStack{SymbolTable()};
};
}  // namespace Vought

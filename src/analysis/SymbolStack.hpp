#pragma once

// parl
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>

// std
#include <list>

namespace PArL {

class SymbolStack {
   public:
    SymbolStack& pushScope();
    SymbolStack& popScope();

    SymbolStack& setType(SymbolTable::Type type);
    SymbolStack& setName(std::string const& name);

    SymbolTable* currentScope();

    [[nodiscard]] bool isCurrentScopeGlobal() const;

   private:
    std::list<SymbolTable> mStack{SymbolTable()};
};

}  // namespace PArL

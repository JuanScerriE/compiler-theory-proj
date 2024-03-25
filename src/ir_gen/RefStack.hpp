#pragma once

// parl
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>

// std
#include <list>

namespace PArL {

class RefStack {
   public:
    RefStack& pushFrame();
    RefStack& popFrame();

    RefStack& setType(SymbolTable::Type type);
    RefStack& setName(std::string const& name);

    SymbolTable* currentScope();

    [[nodiscard]] bool isCurrentScopeGlobal() const;

   private:
    std::list<SymbolTable> mStack{SymbolTable()};
};

}  // namespace PArL

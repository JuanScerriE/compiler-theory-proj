#pragma once

// vought
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>

// std
#include <exception>
#include <list>

namespace Vought {

class SymbolException : public std::exception {};

class SymbolStack {
   public:
    void addIdentifier(std::string const& identifier,
                       Signature signature);

    std::optional<Signature> findIdentifier(
        std::string const& identifier) const;

    void pushScope();
    void popScope();

    SymbolTable& currentScope();

    bool isCurrentScopeGlobal() const;

   private:
    std::list<SymbolTable> mStack{SymbolTable()};
};
}  // namespace Vought

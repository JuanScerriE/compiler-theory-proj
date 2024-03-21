#pragma once

// vought
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>

// std
#include <initializer_list>
#include <list>

namespace Vought {

class SymbolStack {
   public:
    void addIdentifier(std::string const& identifier,
                       Signature signature);

    std::optional<Signature> findIdentifier(
        std::string const& identifier) const;

    std::optional<std::pair<std::string, Signature>>
    getEnclosingFunction() const;

    void pushScope();
    void pushScope(std::initializer_list<Rule> insertRules,
                   std::initializer_list<Rule> searchRules);
    void popScope();

    SymbolTable& currentScope();

    bool isCurrentScopeGlobal() const;

   private:
    std::list<SymbolTable> mStack{SymbolTable()};
};
}  // namespace Vought

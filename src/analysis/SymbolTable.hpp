#pragma once

// vought
#include <analysis/Signature.hpp>

// std
#include <optional>
#include <string>
#include <unordered_map>

namespace Vought {

class SymbolTable {
   public:
    void addIdentifier(std::string const& identifier,
                       Signature signature);
    std::optional<Signature> findIdenfitier(
        std::string const& identifier) const;

   private:
    std::unordered_map<std::string, Signature> mMap{};
};

}  // namespace Vought

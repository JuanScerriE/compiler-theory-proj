#pragma once

// parl
#include <analysis/Signature.hpp>

// std
#include <optional>
#include <string>
#include <unordered_map>

namespace PArL {

class SymbolTable {
   public:
    enum class Type {
        GLOBAL,
        IF,
        ELSE,
        FOR,
        WHILE,
        FUNCTION,
        BLOCK
    };

    void addIdentifier(std::string const& identifier,
                       Signature const& signature);
    [[nodiscard]] std::optional<Signature> findIdentifier(
        std::string const& identifier) const;

    [[nodiscard]] SymbolTable* getEnclosing() const;
    void setEnclosing(SymbolTable* enclosing);

    [[nodiscard]] Type getType() const;
    void setType(Type type);

    [[nodiscard]] std::optional<std::string> getName()
        const;
    void setName(std::string name);

    [[nodiscard]] bool isGlobalScope() const;

   private:
    std::unordered_map<std::string, Signature> mMap{};

    Type mType{Type::GLOBAL};

    std::optional<std::string> mName{};

    SymbolTable* mEnclosing{nullptr};
};

}  // namespace PArL

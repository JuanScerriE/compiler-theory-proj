#pragma once

// parl
#include <ir_gen/SymbolInfo.hpp>

// std
#include <optional>
#include <string>
#include <unordered_map>

namespace PArL {

class Frame {
   public:
    explicit Frame(size_t size);

    void addSymbol(
        std::string const& symbol, SymbolInfo const& info
    );

    [[nodiscard]] std::optional<SymbolInfo> findSymbol(
        std::string const& symbol
    ) const;

    void setEnclosing(Frame* enclosing);
    [[nodiscard]] Frame* getEnclosing() const;

    size_t getIdx() const;

   private:
    std::unordered_map<std::string, SymbolInfo> mMap{};
    const size_t mSize;
    size_t mIdx{0};
    Frame* mEnclosing{nullptr};
};

}  // namespace PArL

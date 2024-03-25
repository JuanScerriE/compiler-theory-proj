#pragma once

// std
#include <cstddef>
#include <optional>
#include <variant>

namespace PArL {

struct VariableInfo {
    size_t idx;
};

struct FunctionInfo {
    size_t labelPos;
    size_t arity;
};

struct SymbolInfo {
    template <typename T>
    [[nodiscard]] bool is() const {
        return std::holds_alternative<T>(data);
    }

    template <typename T>
    [[nodiscard]] std::optional<T> as() const {
        if (std::holds_alternative<T>(data)) {
            return std::get<T>(data);
        }

        return {};
    }

    template <typename T>
    SymbolInfo(T const& info) : data(info) {
    }

    template <typename T>
    SymbolInfo& operator=(T const& info) {
        data = info;

        return *this;
    }

    std::variant<VariableInfo, FunctionInfo> data;
};

}  // namespace PArL

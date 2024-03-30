#pragma once

// std
#include <variant>
#include <vector>

// parl
#include <parl/Core.hpp>

namespace PArL {

struct VariableSymbol {
    size_t idx{0};
    core::Primitive type;

    VariableSymbol(core::Primitive type)
        : type(std::move(type)) {
    }
};

struct FunctionSymbol {
    size_t labelPos{0};
    size_t arity{0};
    std::vector<core::Primitive> paramTypes;
    core::Primitive returnType;

    FunctionSymbol(
        std::vector<core::Primitive> paramTypes,
        core::Primitive returnType
    )
        : paramTypes(std::move(paramTypes)),
          returnType(std::move(returnType)) {
    }
};

struct Symbol {
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
    [[nodiscard]] T& asRef() {
        if (std::holds_alternative<T>(data)) {
            return std::get<T>(data);
        }

        core::abort(
            "asRef conversion to {} failed",
            typeid(T).name()
        );
    }

    explicit Symbol() = default;

    template <typename T>
    Symbol(T const& data)
        : data(data) {
    }

    template <typename T>
    Symbol& operator=(T const& data_) {
        data = data_;

        return *this;
    }

    std::variant<
        std::monostate,
        VariableSymbol,
        FunctionSymbol>
        data{};
};

}  // namespace PArL

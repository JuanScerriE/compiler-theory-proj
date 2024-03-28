#pragma once

// std
#include <variant>
#include <vector>

// parl
#include <parl/Core.hpp>
#include <parl/Token.hpp>

namespace PArL {

struct PrimitiveSig {
    core::Primitive type;
};

struct FunctionSig {
    std::vector<core::Primitive> paramTypes;
    core::Primitive returnType;
};

struct Signature {
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

    explicit Signature() = default;

    template <typename T>
    Signature(T const& info)
        : data(info) {
    }

    template <typename T>
    Signature& operator=(T const& info) {
        data = info;

        return *this;
    }

    bool operator==(Signature const& other) const;
    bool operator!=(Signature const& other) const;

    std::variant<
        std::monostate,
        PrimitiveSig,
        FunctionSig>
        data{};
};

}  // namespace PArL

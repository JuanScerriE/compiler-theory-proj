#pragma once

// std
#include <variant>
#include <vector>

// vought
#include <common/Token.hpp>

namespace PArL {

enum class FundamentalType {
    FLOAT,
    INTEGER,
    BOOL,
    COLOR,
};

struct LiteralSignature {
    FundamentalType type;
};

struct FunctionSignature {
    std::vector<FundamentalType> paramTypes;
    FundamentalType returnType;
};

struct Signature {
    enum class Type {
        LITERAL,
        FUNCTION
    };

    [[nodiscard]] static FundamentalType tokenToType(
        const Token& token);

    [[nodiscard]] static Signature createLiteralSignature(
        Token const& token);
    [[nodiscard]] static Signature createFunctionSignature(
        std::vector<Token> const& params, Token const& ret);

    [[nodiscard]] Type getType() const;

    template <typename T>
    [[nodiscard]] bool is() const {
        return std::holds_alternative<T>(mData);
    }

    template <typename T>
    [[nodiscard]] T as() const {
        if (std::holds_alternative<T>(mData)) {
            return std::get<T>(mData);
        }

        ABORT("accessing signature as inappropriate type");
    }

    Signature& operator=(FundamentalType const& type);

    bool operator==(FundamentalType const& type) const;
    bool operator!=(FundamentalType const& type) const;
    bool operator==(Signature const& other) const;
    bool operator!=(Signature const& other) const;

    Type mType;

    std::variant<LiteralSignature, FunctionSignature> mData;
};

}  // namespace PArL

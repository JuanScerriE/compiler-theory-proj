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

enum class SignatureType {
    VARIABLE,
    FUNCTION
};

struct LiteralSignature {
    FundamentalType type;

    static LiteralSignature fromTokenType(
        Token const& token) noexcept;

    static LiteralSignature fromLiteral(
        Token const& token) noexcept;
};

struct FunctionSignature {
    std::vector<FundamentalType> paramTypes;

    FundamentalType returnType;

    static FunctionSignature fromParamsAndReturnTypes(
        std::vector<Token> const& params,
        Token const& ret) noexcept;
};

struct Signature {
    std::variant<LiteralSignature, FunctionSignature>
        data{};

    Signature() = default;

    explicit Signature(LiteralSignature sig);
    explicit Signature(FunctionSignature sig);

    [[nodiscard]] SignatureType getType() const;

    [[nodiscard]] FunctionSignature const& asFunctionSig()
        const;

    [[nodiscard]] bool isLiteralSig() const;
    [[nodiscard]] bool isFunctionSig() const;

    [[nodiscard]] bool isSameSig(
        LiteralSignature const& other) const;
    [[nodiscard]] bool isSameSig(
        FunctionSignature const& other) const;

    bool operator==(FundamentalType const& type) const;
    bool operator!=(FundamentalType const& type) const;
    bool operator==(Signature const& other) const;
    bool operator!=(Signature const& other) const;

    SignatureType type{};
};

}  // namespace PArL

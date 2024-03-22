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
        Token token) noexcept;

    static LiteralSignature fromLiteral(
        Token token) noexcept;
};

struct FunctionSignature {
    std::vector<FundamentalType> paramTypes;

    FundamentalType returnType;

    static FunctionSignature fromParamsAndReturnTypes(
        std::vector<Token> params, Token ret) noexcept;
};

struct Signature {
    std::variant<LiteralSignature, FunctionSignature>
        data{};

    Signature() = default;

    Signature(LiteralSignature sig);
    Signature(FunctionSignature sig);

    SignatureType getType() const;

    FunctionSignature const& asFunctionSig() const;

    bool isLiteralSig() const;
    bool isFunctionSig() const;

    bool isSameSig(LiteralSignature const& other) const;
    bool isSameSig(FunctionSignature const& other) const;

    bool operator==(FundamentalType const& type) const;
    bool operator!=(FundamentalType const& type) const;
    bool operator==(Signature const& other) const;
    bool operator!=(Signature const& other) const;

    SignatureType type{};
};

}  // namespace PArL

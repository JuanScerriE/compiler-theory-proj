#pragma once

// std
#include <variant>
#include <vector>

// vought
#include <common/Token.hpp>

namespace Vought {

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

struct VariableSignature {
    FundamentalType type;

    static VariableSignature fromTokenType(
        Token token) noexcept;
};

struct FunctionSignature {
    std::vector<FundamentalType> paramTypes;

    FundamentalType returnType;

    static FunctionSignature fromParamsAndReturnTypes(
        std::vector<Token> params, Token ret) noexcept;
};

struct Signature {
    std::variant<VariableSignature, FunctionSignature> data;

    Signature(VariableSignature sig);
    Signature(FunctionSignature sig);

    SignatureType getType() const;

    bool isVariableSig() const;
    bool isFunctionSig() const;

    bool isSameSig(VariableSignature const& other) const;
    bool isSameSig(FunctionSignature const& other) const;

    SignatureType type;
};

}  // namespace Vought

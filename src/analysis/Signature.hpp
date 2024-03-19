#pragma once

// std
#include <variant>
#include <vector>

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
};

struct FunctionSignature {
    std::vector<FundamentalType> paramTypes;

    FundamentalType returnType;
};

struct Signature {
    std::variant<VariableSignature, FunctionSignature> data;

    SignatureType getType() const;

    bool isSameSig(VariableSignature const& other) const;
    static Signature createVariableSig();
    bool isSameSig(FunctionSignature const& other) const;
    static Signature createFunctionSig();
};

}  // namespace Vought

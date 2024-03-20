#include <analysis/Signature.hpp>
#include <common/Abort.hpp>

namespace Vought {

Signature::Signature(VariableSignature sig)
    : data(sig), type(SignatureType::VARIABLE) {
}

Signature::Signature(FunctionSignature sig)
    : data(sig), type(SignatureType::FUNCTION) {
}

FundamentalType convertToFundamentalType(Token tok) {
    switch (tok.getType()) {
        case Token::Type::FLOAT_TYPE:
            return FundamentalType::FLOAT;
        case Token::Type::INTEGER_TYPE:
            return FundamentalType::INTEGER;
        case Token::Type::BOOL_TYPE:
            return FundamentalType::BOOL;
        case Token::Type::COLOR_TYPE:
            return FundamentalType::COLOR;
        default:
            ABORTIF(true, fmt::format("token type is not a "
                                      "fundamental type {}",
                                      tok.toString(true)));
    }
}

bool Signature::isVariableSig() const {
    return type == SignatureType::VARIABLE;
}

bool Signature::isFunctionSig() const {
    return type == SignatureType::FUNCTION;
}

VariableSignature VariableSignature::fromTokenType(
    Token token) noexcept {
    return {convertToFundamentalType(token)};
}

FunctionSignature
FunctionSignature::fromParamsAndReturnTypes(
    std::vector<Token> params, Token ret) noexcept {
    std::vector<FundamentalType> paramTypes;

    for (auto& param : params) {
        paramTypes.push_back(
            convertToFundamentalType(param));
    }

    return {paramTypes, convertToFundamentalType(ret)};
}

}  // namespace Vought

#include <analysis/Signature.hpp>
#include <common/Abort.hpp>

namespace PArL {

Signature::Signature(LiteralSignature sig)
    : data(sig), type(SignatureType::VARIABLE) {
}

Signature::Signature(FunctionSignature sig)
    : data(sig), type(SignatureType::FUNCTION) {
}

FundamentalType convertToFundamentalType(
    Token const& token) {
    switch (token.getType()) {
        case Token::Type::FLOAT_TYPE:
            return FundamentalType::FLOAT;
        case Token::Type::INTEGER_TYPE:
            return FundamentalType::INTEGER;
        case Token::Type::BOOL_TYPE:
            return FundamentalType::BOOL;
        case Token::Type::COLOR_TYPE:
            return FundamentalType::COLOR;
        default:
            ABORT(
                fmt::format("token type is not a "
                            "fundamental type {}",
                            token.toString()));
    }
}

bool Signature::isLiteralSig() const {
    return type == SignatureType::VARIABLE;
}

bool Signature::isFunctionSig() const {
    return type == SignatureType::FUNCTION;
}

LiteralSignature LiteralSignature::fromTokenType(
    Token const& token) noexcept {
    return {convertToFundamentalType(token)};
}

LiteralSignature LiteralSignature::fromLiteral(
    Token const& token) noexcept {
    switch (token.getType()) {
        case Token::Type::BOOL:
            return {FundamentalType::BOOL};
        case Token::Type::INTEGER:
            return {FundamentalType::INTEGER};
        case Token::Type::FLOAT:
            return {FundamentalType::FLOAT};
        case Token::Type::COLOR:
            return {FundamentalType::COLOR};
        default:
            ABORTIF(true, fmt::format("token type is not a "
                                      "fundamental type {}",
                                      token.toString()));
    }
}

FunctionSignature const& Signature::asFunctionSig() const {
    ABORTIF(!isFunctionSig(),
            "signature is not a function signature");

    return std::get<FunctionSignature>(data);
}

FunctionSignature
FunctionSignature::fromParamsAndReturnTypes(
    std::vector<Token> const& params,
    Token const& ret) noexcept {
    std::vector<FundamentalType> paramTypes;

    for (auto& param : params) {
        paramTypes.push_back(
            convertToFundamentalType(param));
    }

    return {paramTypes, convertToFundamentalType(ret)};
}

bool Signature::operator!=(
    FundamentalType const& type) const {
    return !(operator==(type));
}

bool Signature::operator==(
    FundamentalType const& type) const {
    if (isLiteralSig()) {
        return std::get<LiteralSignature>(data).type ==
               type;
    }

    return false;
}

bool Signature::operator==(Signature const& other) const {
    if (isLiteralSig() && other.isLiteralSig()) {
        return std::get<LiteralSignature>(data).type ==
               std::get<LiteralSignature>(other.data).type;
    }

    if (isFunctionSig() && other.isFunctionSig()) {
        FunctionSignature thisFunc =
            std::get<FunctionSignature>(data);
        FunctionSignature otherFunc =
            std::get<FunctionSignature>(other.data);

        if (thisFunc.paramTypes.size() !=
            otherFunc.paramTypes.size()) {
            return false;
        }

        for (int i = 0; i < thisFunc.paramTypes.size();
             i++) {
            if (thisFunc.paramTypes[i] !=
                otherFunc.paramTypes[i]) {
                return false;
            }
        }

        if (thisFunc.returnType != otherFunc.returnType) {
            return false;
        }

        return true;
    }

    return false;
}

bool Signature::operator!=(Signature const& other) const {
    return !(operator==(other));
}
}  // namespace PArL

#include <analysis/Signature.hpp>
#include <common/Abort.hpp>

namespace PArL {

FundamentalType Signature::tokenToType(const Token& token) {
    switch (token.getType()) {
        case Token::Type::FLOAT:
        case Token::Type::FLOAT_TYPE:
            return FundamentalType::FLOAT;
        case Token::Type::INTEGER:
        case Token::Type::INTEGER_TYPE:
            return FundamentalType::INTEGER;
        case Token::Type::BOOL:
        case Token::Type::BOOL_TYPE:
            return FundamentalType::BOOL;
        case Token::Type::COLOR:
        case Token::Type::COLOR_TYPE:
            return FundamentalType::COLOR;
        default:
            ABORT(
                "token type is not a "
                "fundamental type {}",
                token.toString());
    }
}

Signature Signature::createLiteralSignature(
    const Token& token) {
    return {Signature::Type::LITERAL,
            LiteralSignature{tokenToType(token)}};
}

Signature Signature::createFunctionSignature(
    const std::vector<Token>& params, const Token& ret) {
    std::vector<FundamentalType> paramTypes{params.size()};

    for (int i = 0; i < params.size(); i++) {
        paramTypes[i] = tokenToType(params[i]);
    }

    return {
        Signature::Type::FUNCTION,
        FunctionSignature{paramTypes, tokenToType(ret)}};
}

Signature& Signature::operator=(
    FundamentalType const& type) {
    mType = Signature::Type::LITERAL;

    mData = LiteralSignature{type};

    return *this;
}

bool Signature::operator==(
    FundamentalType const& type) const {
    if (is<LiteralSignature>()) {
        return as<LiteralSignature>().type == type;
    }

    return false;
}

bool Signature::operator!=(
    FundamentalType const& type) const {
    return !(operator==(type));
}

bool Signature::operator==(Signature const& other) const {
    if (is<LiteralSignature>() &&
        other.is<LiteralSignature>()) {
        return as<LiteralSignature>().type ==
               other.as<LiteralSignature>().type;
    }

    if (is<FunctionSignature>() &&
        other.is<FunctionSignature>()) {
        auto thisFunc = as<FunctionSignature>();
        auto otherFunc = other.as<FunctionSignature>();

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

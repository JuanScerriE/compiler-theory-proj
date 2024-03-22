#include <analysis/Signature.hpp>
#include <common/Abort.hpp>

namespace PArL {

Signature Signature::createVariableSignature(
    const Token& token) {
}

Signature Signature::createFunctionSignature(
    const std::vector<Token>& params, const Token& ret) {
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

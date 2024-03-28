// parl
#include <analysis/Signature.hpp>

namespace PArL {

bool Signature::operator==(Signature const& other) const {
    if (is<PrimitiveSig>() && other.is<PrimitiveSig>()) {
        auto sig = *as<PrimitiveSig>();
        auto otherSig = *other.as<PrimitiveSig>();

        return sig.type == otherSig.type;
    }

    if (is<FunctionSig>() && other.is<FunctionSig>()) {
        auto sig = *as<FunctionSig>();
        auto otherSig = *other.as<FunctionSig>();

        if (sig.paramTypes.size() !=
            otherSig.paramTypes.size()) {
            return false;
        }

        for (size_t i = 0; i < sig.paramTypes.size(); i++) {
            if (sig.paramTypes[i] !=
                otherSig.paramTypes[i]) {
                return false;
            }
        }

        if (sig.returnType != otherSig.returnType) {
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

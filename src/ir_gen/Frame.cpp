// parl
#include <common/Abort.hpp>
#include <ir_gen/Frame.hpp>

namespace PArL {

Frame::Frame(size_t size)
    : mSize(size) {
}

void Frame::addSymbol(
    std::string const& symbol,
    SymbolInfo const& info
) {
    abortIf(mIdx + 1 > mSize, "frame size exceeded");

    mIdx++;

    mMap.insert({symbol, info});
}

std::optional<SymbolInfo> Frame::findSymbol(
    std::string const& symbol
) const {
    if (mMap.count(symbol) > 0) {
        return mMap.at(symbol);
    }

    return {};
}

Frame* Frame::getEnclosing() const {
    return mEnclosing;
}

void Frame::setEnclosing(Frame* enclosing) {
    mEnclosing = enclosing;
}

size_t Frame::getIdx() const {
    return mIdx;
}

}  // namespace PArL

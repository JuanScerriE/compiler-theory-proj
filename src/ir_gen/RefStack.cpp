// parl
#include <ir_gen/RefStack.hpp>

namespace PArL {

RefStack& RefStack::pushFrame(size_t size) {
    Frame* enclosing = nullptr;

    if (!mFrames.empty())
        enclosing = currentFrame();

    mFrames.emplace_front(size);

    currentFrame()->setEnclosing(enclosing);

    return *this;
}

RefStack& RefStack::popFrame() {
    mFrames.pop_front();

    return *this;
}

Frame* RefStack::currentFrame() {
    return &mFrames.front();
}

}  // namespace PArL

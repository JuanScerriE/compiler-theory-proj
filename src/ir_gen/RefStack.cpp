// parl

#include <ir_gen/RefStack.hpp>

namespace PArL {

RefStack& RefStack::pushFrame() {
    size_t index = mStack.top();

    mStack.top()++;

    mStack.push(0);

    mCurrent = mCurrent->children()[index].get();

    return *this;
}

RefStack& RefStack::pushFrame(size_t size) {
    size_t index = mStack.top();

    mStack.top()++;

    mStack.push(0);

    mCurrent = mCurrent->children()[index].get();

    mCurrent->setSize(size);

    return *this;
}

Environment* RefStack::peekNextFrame() {
    return mCurrent->children()[mStack.top()].get();
}

RefStack& RefStack::popFrame() {
    mStack.pop();

    mCurrent = mCurrent->getEnclosing();

    return *this;
}

Environment* RefStack::currentFrame() {
    return mCurrent;
}

void RefStack::init(Environment* global) {
    reset();
    mGlobal = global;
    mCurrent = global;
}

void RefStack::init(
    Environment* global,
    Environment* current
) {
    reset();
    mGlobal = global;
    mCurrent = current;
}

void RefStack::reset() {
    mGlobal = nullptr;
    mCurrent = nullptr;

    while (!mStack.empty())
        mStack.pop();

    mStack.push(0);
}

}  // namespace PArL

#pragma once

// parl
#include <backend/Environment.hpp>

// std
#include <list>

namespace PArL {

class RefStack {
   public:
    RefStack& pushFrame();
    RefStack& pushFrame(size_t size);
    Environment* peekNextFrame();
    RefStack& popFrame();

    [[nodiscard]] Environment* currentFrame();

    void init(Environment* global);
    void init(Environment* global, Environment* current);

    void reset();

   private:
    Environment* mGlobal{nullptr};
    Environment* mCurrent{nullptr};
    std::stack<size_t> mStack{{0}};
};

}  // namespace PArL

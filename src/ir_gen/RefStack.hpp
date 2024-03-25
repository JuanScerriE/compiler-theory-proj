#pragma once

// parl
#include <ir_gen/Frame.hpp>

// std
#include <list>

namespace PArL {

class RefStack {
   public:
    RefStack& pushFrame(size_t size);
    RefStack& popFrame();

    [[nodiscard]] Frame* currentFrame();

   private:
    std::list<Frame> mFrames{};
};

}  // namespace PArL

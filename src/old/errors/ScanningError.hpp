#pragma once

// std
#include <exception>

// lox
#include <common/Token.hpp>

namespace Lox {

class ScanningError : public std::exception {
   public:
    ScanningError(int line, char const* message)
        : mLine(line), mMessage(message) {
    }

    [[nodiscard]] int getLine() const {
        return mLine;
    }

    [[nodiscard]] const char* what()
        const noexcept override {
        return mMessage;
    }

   private:
    int mLine;
    const char* mMessage;
};

}  // namespace Lox

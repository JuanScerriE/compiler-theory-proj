#pragma once

// std
#include <exception>

// lox
#include <common/Token.hpp>
#include <utility>

namespace Lox {

class RuntimeError : public std::exception {
   public:
    RuntimeError(Token const &token, std::string message)
        : mToken(token), mMessage(std::move(message)) {
    }
    RuntimeError(Token const &token, char const *message)
        : mToken(token), mMessage(message) {
    }

    [[nodiscard]] Token const &getToken() const {
        return mToken;
    }
    [[nodiscard]] char const *what()
        const noexcept override {
        return mMessage.c_str();
    }

   private:
    Token const &mToken;
    std::string mMessage;
};

}  // namespace Lox

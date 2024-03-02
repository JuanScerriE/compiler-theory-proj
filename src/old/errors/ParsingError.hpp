#pragma once

// std
#include <exception>

// lox
#include <common/Token.hpp>
#include <utility>

namespace Lox {

class ParsingError : public std::exception {
   public:
    ParsingError(Token& token, char const* message)
        : mToken(token), mMessage(message) {
    }

    ParsingError(Token& token, std::string message)
        : mToken(token), mMessage(std::move(message)) {
    }

    [[nodiscard]] Token& getToken() const {
        return mToken;
    }

    [[nodiscard]] char const* what()
        const noexcept override {
        return mMessage.c_str();
    }

   private:
    Token& mToken;
    std::string mMessage;
};

}  // namespace Lox

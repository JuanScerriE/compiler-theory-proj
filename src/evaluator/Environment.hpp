#pragma once

// std
#include <string>
#include <unordered_map>

// lox
#include <common/Token.hpp>
#include <common/Value.hpp>

namespace Lox {

class Environment {
   public:
    void define(std::string const& name,
                Value const& value);
    [[nodiscard]] Value get(Token const& name) const;

   private:
    std::unordered_map<std::string, Value> mValues;
};

}  // namespace Lox

#pragma once

// std
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

// parl
#include <parl/Core.hpp>

namespace PArL {

static const std::unordered_map<std::string, core::Builtin>
    builtins{
        {"__width", core::Builtin::WIDTH},
        {"__height", core::Builtin::HEIGHT},
        {"__read", core::Builtin::READ},
        {"__random_int", core::Builtin::RANDOM_INT},
        {"__print", core::Builtin::PRINT},
        {"__delay", core::Builtin::DELAY},
        {"__write", core::Builtin::WRITE},
        {"__write_box", core::Builtin::WRITE_BOX},
        {"__clear", core::Builtin::CLEAR},
    };

struct Value {
    template <typename T>
    [[nodiscard]] bool is() const {
        return std::holds_alternative<T>(data);
    }

    template <typename T>
    [[nodiscard]] std::optional<T> as() const {
        if (std::holds_alternative<T>(data)) {
            return std::optional<T>{std::get<T>(data)};
        }

        return {};
    }

    template <typename T>
    static Value create(const std::string&) {
        core::abort("unimplemented");
    }

    template <>
    Value create<bool>(const std::string& lexeme) {
        return {lexeme == "true"};
    }

    template <>
    Value create<core::Color>(const std::string& lexeme) {
        return {core::Color{
            static_cast<uint8_t>(lexeme[1] + lexeme[2]),
            static_cast<uint8_t>(lexeme[3] + lexeme[4]),
            static_cast<uint8_t>(lexeme[5] + lexeme[6])
        }};
    }

    template <>
    Value create<float>(const std::string& lexeme) {
        return {std::stof(lexeme)};
    }

    template <>
    Value create<int>(const std::string& lexeme) {
        return {std::stoi(lexeme)};
    }

    template <>
    Value create<core::Builtin>(const std::string& lexeme) {
        return {builtins.at(lexeme)};
    }

    template <>
    Value create<std::string>(const std::string& lexeme) {
        return {lexeme};
    }

    std::variant<
        float,
        int,
        core::Color,
        bool,
        core::Builtin,
        std::string>
        data;
};

}  // namespace PArL

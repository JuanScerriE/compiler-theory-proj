#pragma once

// fmt
#include <fmt/core.h>

// std
#include <string>

#define STRINGIFY_(x) #x

#define STRINGIFY(x) STRINGIFY_(x)

#define LINE_STRING STRINGIFY(__LINE__)

namespace PArL {

#ifdef NDEBUG
template <typename... T>
inline void abort(fmt::format_string<T...>, T&&...) {
}

template <typename... T>
inline void abortIf(bool, fmt::format_string<T...>, T&&...) {
}
#else
template <typename... T>
inline void abort(fmt::format_string<T...> fmt, T&&... args) {
    fmt::println(stderr, fmt, args...); std::abort();
}

template <typename... T>
inline void abortIf(bool cond, fmt::format_string<T...> fmt, T&&... args) {
    if (cond) {
        abort(fmt, args...);
    }
}
#endif

} // namespace PArL

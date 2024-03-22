#pragma once

// fmt
#include <fmt/core.h>

// std
#include <string>

#define STRINGIFY_(x) #x

#define STRINGIFY(x) STRINGIFY_(x)

#define LINE_STRING STRINGIFY(__LINE__)

#ifdef NDEBUG
#define INTERNAL_DEBUG 0
#else
#define INTERNAL_DEBUG 1
#endif

inline void abortIf(bool condition,
                    std::string const& message) {
    if (INTERNAL_DEBUG && condition) {
        fmt::println(stderr, message);

        std::abort();
    }
}

#define ABORTIF(condition, message)                        \
    (abortIf(condition,                                    \
             fmt::format(__FILE__ ":" LINE_STRING ":: {}", \
                         message)))

#define ABORT(message)                                     \
    (abortIf(true,                                         \
             fmt::format(__FILE__ ":" LINE_STRING ":: {}", \
                         message)))

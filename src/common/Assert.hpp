#pragma once

// fmt
#include <fmt/core.h>

// std
#include <string>

#define STRINGIZE_(x) #x

#define STRINGIZE(x) STRINGIZE_(x)

#define LINE_STRING STRINGIZE(__LINE__)

#ifdef NDEBUG
#define INTERNAL_DEBUG 0
#else
#define INTERNAL_DEBUG 1
#endif

inline void abortif(bool condition, std::string message) {
    if (INTERNAL_DEBUG && condition) {
        fmt::println(message);

        std::abort();
    }
}

#define ABORTIF(condition, message)                        \
    (abortif(condition,                                    \
             fmt::format(__FILE__ ":" LINE_STRING ":: {}", \
                         message)))

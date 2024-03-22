#pragma once

// fmt
#include <fmt/core.h>

// std
#include <string>

#define STRINGIFY_(x) #x

#define STRINGIFY(x) STRINGIFY_(x)

#define LINE_STRING STRINGIFY(__LINE__)

#ifndef NDEBUG
#define INTERNAL_DEBUG
#endif

#ifdef INTERNAL_DEBUG
inline void abortIf(bool condition,
                    std::string const& message) {
    if (condition) {
        fmt::println(stderr, message);

        std::abort();
    }
}
#else
inline void abortIf(bool condition,
                    std::string const& message) {
    ((void)condition);
    ((void)message);
}
#endif

#define ABORTIF(condition, message)                       \
    do {                                                  \
        abortIf(                                          \
            condition,                                    \
            fmt::format(__FILE__ ":" LINE_STRING ":: {}", \
                        message));                        \
    } while (0);

#define ABORT(message)                                     \
    do {                                                   \
        abortIf(true, fmt::format(__FILE__ ":" LINE_STRING \
                                           ":: {}",        \
                                  message));               \
    } while (0);

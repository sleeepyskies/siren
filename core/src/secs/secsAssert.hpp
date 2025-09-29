#pragma once
#include "slog.hpp"
#define SECS_ASSERT(cond, msg)                                                                                         \
    do {                                                                                                               \
        if (!(cond)) {                                                                                                 \
            err("secs Assertion failed: {}", msg);                                                                     \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)

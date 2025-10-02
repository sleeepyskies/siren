#pragma once

#include "slog.hpp"

#define SirenAssert(cond, fmt, ...)                                                                \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            err("Siren Assert: " fmt __VA_OPT__(, ) __VA_ARGS__);                                  \
            std::abort();                                                                          \
        }                                                                                          \
    } while (0)

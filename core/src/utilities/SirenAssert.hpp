/**
 * @file SirenAssert.hpp
 * @brief Contains useful assertion macros
 */
#pragma once

#include "slog.hpp"

/// @brief Checks the given condition and crashes the program if false and prints the debug message.
#define SirenAssert(cond, fmt, ...)                                                                \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            err("Siren Assert: " fmt __VA_OPT__(, ) __VA_ARGS__);                                  \
            std::abort();                                                                          \
        }                                                                                          \
    } while (0)

/// @brief Crashes the program if called. Should be used to indicate missing functionality.
#define NotImplemented SirenAssert(false, "Not Implemented")

#define Todo err("Encountered Todo code. Continuing as not fatal.")

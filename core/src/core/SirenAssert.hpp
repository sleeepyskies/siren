#pragma once
#include "slog.hpp"

/// @brief Checks the given condition and crashes the program if false and prints the debug message.
#define SIREN_ASSERT(cond, fmt, ...)                                                                \
     do {                                                                                           \
         if (!(cond)) {                                                                             \
             err("Siren Assert: " fmt __VA_OPT__(, ) __VA_ARGS__);                                  \
             std::abort();                                                                          \
     }                                                                                              \
} while (0)

/// @brief Crashes the program if called. Should be used to indicate missing functionality.
#define NOT_IMPLEMENTED SIREN_ASSERT(false, "Not Implemented")
/// @brief Crashes the program if called. Should be used to indicate some unreachable state.
#define UNREACHABLE                                                                                 \
    err("Unreachable");                                                                             \
    std::unreachable();                                                                             \
/// @brief Outputs to log about unfinished code, but doesn't crash the program.
#define TODO err("Encountered Todo code. Continuing as not fatal.")

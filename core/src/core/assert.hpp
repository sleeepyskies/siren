#pragma once

#include "spdlog/spdlog.h"
#include "core/Logger.hpp"

/// @brief Checks the given condition and crashes the program if false and prints the debug message.
#define SIREN_ASSERT(cond, fmt, ...)                                                                \
     do {                                                                                           \
         if (!(cond)) {                                                                             \
             siren::core::Logger::core->critical("assertion fail: " fmt );                          \
             spdlog::shutdown();                                                                    \
             std::abort();                                                                          \
     }                                                                                              \
} while (0)

/// @brief Outputs to log about unfinished code, but doesn't crash the program.
#define TODO                                                                                        \
    siren::core::Logger::core->critical("todo");                                                    \
    spdlog::shutdown();                                                                             \
    std::abort();
/// @brief Crashes the program if called. Should be used to indicate some unreachable state.
#define UNREACHABLE                                                                                 \
    siren::core::Logger::core->critical("unreachable");                                             \
    spdlog::shutdown();                                                                             \
    std::unreachable();

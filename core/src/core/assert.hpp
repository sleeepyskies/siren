#pragma once

#include "spdlog/spdlog.h"


namespace siren::core::detail
{
auto report_assert(std::string_view fmt, std::string_view file, i32 line) -> void;
}


/**
 * @brief Asserts a given condition and exits the program on fail with an error message.
 *
 * In some places, we cannot use the Logger. Therefore, we provide a fallback using
 * std::println.
 *
 * @param cond The condition to check.
 * @param fmt The format string.
 * @param ... Variadic arguments to format.
 */
#define SIREN_ASSERT(cond, fmt, ...)                                                                \
    if (!(cond)) [[unlikely]] {                                                                     \
        auto msg = std::format(fmt, ##__VA_ARGS__);                                                 \
        siren::core::detail::report_assert(msg, __FILE__, __LINE__);                              \
    }

/// @brief Crashes the program with a todo message.
#define TODO                                                                                        \
    SIREN_ASSERT(false, "todo");

/// @brief Crashes the program with an unreachable message.
#define UNREACHABLE                                                                                 \
    SIREN_ASSERT(false, "unreachable");

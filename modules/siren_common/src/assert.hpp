#pragma once

#include <functional>
#include <source_location>
#include <fmt/format.h>


namespace siren::detail {
using CrashHandler                  = std::function<void(std::source_location, const std::string&)>;
inline CrashHandler g_crash_handler = nullptr;
auto report_assert(const std::string& msg, std::source_location sl) -> void;
} // namespace siren::detail

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
#define SIREN_ASSERT(cond, fmt_str, ...)                                                    \
    do {                                                                                    \
        if (!(cond)) [[unlikely]] {                                                         \
            auto msg = fmt::format(fmt_str __VA_OPT__(,)  __VA_ARGS__);                     \
            const auto sl = std::source_location::current();                                \
            detail::report_assert(msg, sl);                                                 \
        }                                                                                   \
    } while (0)

//** @brief Crashes the program with a todo message. */
#define TODO SIREN_ASSERT(false, "todo");

//** @brief Crashes the program with an unreachable message. */
#define UNREACHABLE SIREN_ASSERT(false, "unreachable");

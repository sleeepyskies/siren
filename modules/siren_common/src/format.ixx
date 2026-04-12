module;

#include <fmt/format.h>

export module siren.common:format;

import :concepts;

/**
 * @brief Basic general formatter for any type implementing a to_string() method.
 * @note This general formatter doesn't allow formatting with custom flags.
 * @tparam T The type to format
 */
export template <siren::IsFormattable T>
struct fmt::formatter<T> {
    constexpr auto parse(const format_parse_context& ctx) const {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw format_error("siren type: custom specifiers not supported");
        }
        return it;
    }

    auto format(const T& t, const format_context& ctx) const {
        return fmt::format_to(ctx.out(), t.to_string());
    }
};

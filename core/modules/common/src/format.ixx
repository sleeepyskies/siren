export module siren.common:format;

import <format>;
import <string_view>;

import :typedefs;

export template <siren::IsFormattable Obj>
struct std::formatter<Obj> : std::formatter<std::string_view> {
    auto format(const Obj& obj, std::format_context& ctx) const {
        return std::formatter<std::string_view>::format(obj.to_string(), ctx);
    }
};


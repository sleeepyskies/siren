#pragma once

#include <format>


namespace siren::core
{

enum class Code {
    None = 0,

    // Various core systems
    ResourceLocked,

    // Assets
    AssetNotFound = 100,
    AssetInvalidFormat,
    AssetCorrupted,
    AssetUnsupported,
    AssetIOFail,

    MAX,
};

struct Error {

    Code code;
    String msg;

    explicit Error(const Code code, String&& msg = "") : code(code), msg(msg) { }
};

[[nodiscard]]
constexpr auto error_code_to_string(const Code code) -> std::string_view {
    switch (code) {
        case Code::None: return "None";

        case Code::ResourceLocked: return "ResourceLocked";

        case Code::AssetNotFound: return "AssetNotFound";
        case Code::AssetInvalidFormat: return "AssetInvalidFormat";
        case Code::AssetCorrupted: return "AssetCorrupted";
        case Code::AssetUnsupported: return "AssetUnsupported";
        case Code::AssetIOFail: return "AssetIOFail";

        default: return "UNKNOWN";
    }
}
} // namespace siren::core

template <>
struct std::formatter<siren::core::Error> : std::formatter<std::string_view> {
    auto format(const siren::core::Error& error, std::format_context& ctx) const {
        return std::format_to(
            ctx.out(),
            "[Error::{}]",
            siren::core::error_code_to_string(error.code)
        );
    }
};

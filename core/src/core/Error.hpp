#pragma once

#include <format>


namespace siren::core
{

/**
 * @brief All siren error codes.
 */
enum class Code {
    None = 0, ///< @brief Nothing, shouldn't be used.

    // Various core systems
    ResourceLocked, ///< @brief Attempted to use a shared resource that is currently locked.
    LogicFail,      ///< @brief Some incorrect internal logic.
    IOFail,         ///< @brief Failure during IO.

    // Assets
    AssetNotFound = 100, ///< @brief Could not find the asset.
    AssetCorrupted,      ///< @brief The asset has invalid internal data.
    AssetUnsupported,    ///< @brief The asset is not supported by siren.

    MAX, ///< @brief Max value. Shouldn't be used.
};

/**
 * @brief An error in siren.
 */
struct Error {
    Code code;                      ///< @brief The code of the error.
    std::optional<std::string> msg; ///< @brief An optional error message.

    explicit Error(const Code code, const std::optional<std::string>& msg = std::nullopt) : code(code), msg(msg) { }
};

[[nodiscard]]
constexpr auto error_code_to_string(const Code code) -> std::string_view {
    switch (code) {
        case Code::None: return "None";

        case Code::ResourceLocked: return "ResourceLocked";
        case Code::IOFail: return "IOFail";

        case Code::AssetNotFound: return "AssetNotFound";
        case Code::AssetCorrupted: return "AssetCorrupted";
        case Code::AssetUnsupported: return "AssetUnsupported";

        default: return "UNKNOWN";
    }
}
} // namespace siren::core

template <>
struct std::formatter<siren::core::Error> : std::formatter<std::string_view> {
    auto format(const siren::core::Error& error, std::format_context& ctx) const {
        return std::format_to(
            ctx.out(),
            "[Error::{}] {}",
            siren::core::error_code_to_string(error.code),
            error.msg.value_or("")
        );
    }
};

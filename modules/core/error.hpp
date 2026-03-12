#pragma once

#include "code.hpp"
#include "utilities/string_mappings.hpp"


namespace siren::core
{

/**
 * @brief An error in siren.
 */
struct Error {
    Code code;                      ///< @brief The code of the error.
    std::optional<std::string> msg; ///< @brief An optional error message.
    explicit Error(const Code code, const std::optional<std::string>& msg = std::nullopt) : code(code), msg(msg) { }
};

} // namespace siren::core

template <>
struct std::formatter<siren::core::Error> : std::formatter<std::string_view> {
    auto format(const siren::core::Error& error, std::format_context& ctx) const {
        return std::format_to(
            ctx.out(),
            "[Error::{}] {}",
            siren::utilities::to_string(error.code),
            error.msg.value_or("")
        );
    }
};

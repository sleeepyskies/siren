#include "assert.hpp"

#include "logger.hpp"
#include <iostream>


namespace siren::core::detail
{
auto report_assert(const std::string_view msg, const std::string_view file, const i32 line) -> void {
    if (Locator<Logger>::has_value()) {
        const auto& log = Locator<Logger>::value();
        log.core->critical("ASSERT: {} ({}:{})", msg, file, line);
        spdlog::shutdown();
    } else {
        std::cerr << "FATAL [No Logger]: " << msg
                << " (" << file << ":" << line << ")"
                << std::endl;
    }
    std::abort();
}
} // namespace siren::core::detail

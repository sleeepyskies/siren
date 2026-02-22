#include "assert.hpp"
import <iostream>; // idk why but this makes it work and #include doesn't lmao
#include "logger.hpp"


namespace siren::core::detail
{
auto report_assert(const std::string_view fmt, const std::string_view file, const i32 line) -> void {
    if (Locator<Logger>::has_value()) {
        const auto& log = Locator<Logger>::value();
        log.core->critical("ASSERT: {} ({}:{})", fmt, file, line);
        spdlog::shutdown();
    } else {
        std::println(std::cerr, "FATAL [No Logger]: {} ({}:{})", fmt, file, line);
    }
    std::abort();
}
} // namespace siren::core::detail

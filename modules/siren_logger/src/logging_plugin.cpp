module;

#include "assert.hpp"

module siren.log;

namespace siren::log {

LoggingPlugin::LoggingPlugin(const LogLevel level) : m_level(level) { }

auto LoggingPlugin::construct(App& app) const -> void {
    init(m_level);
    detail::g_crash_handler = [] (const std::source_location sl, const std::string& msg) {
        log(LogLevel::critical, sl, msg);
        std::abort();
    };
}

auto LoggingPlugin::shutdown(App& app) const -> void {
    log::shutdown();
}

} // namespace siren::log

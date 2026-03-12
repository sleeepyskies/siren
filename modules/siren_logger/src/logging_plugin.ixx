module;

#include "assert.hpp"

export module siren.log:logging_plugin;

import :logger;
import siren.app;

namespace siren::log {

class LoggingPlugin final : public Plugin {
public:
    explicit LoggingPlugin(LogLevel level);
    auto construct(App&) const -> void override;
    auto shutdown(App&) const -> void override;
    auto name() const -> std::string_view override;

private:
    LogLevel m_level;
};

LoggingPlugin::LoggingPlugin(const LogLevel level) : m_level(level) { }

auto LoggingPlugin::construct(App&) const -> void {
    init(m_level);
    detail::g_crash_handler = [] (const std::source_location sl, const std::string& msg) {
        log(LogLevel::critical, sl, msg);
        std::abort();
    };
}

auto LoggingPlugin::shutdown(App&) const -> void {
    log::shutdown();
}

auto LoggingPlugin::name() const -> std::string_view {
    return "LoggingPlugin";
}

} // namespace siren::log

module;

#include "assert.hpp"

export module siren.log:logging_plugin;

import :logger;
import siren.app.plugin;

namespace siren {
class App;
}


namespace siren::log {

class LoggingPlugin final : public Plugin {
public:
    explicit LoggingPlugin(LogLevel level);
    ~LoggingPlugin() override = default;
    auto construct(App& app) const -> void override;
    auto shutdown(App& app) const -> void override;

private:
    LogLevel m_level;
};

} // namespace siren::log

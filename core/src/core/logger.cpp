#include "logger.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/async.h"


namespace siren
{
static constexpr auto to_spdlog_level(const std::string& str) -> spdlog::level::level_enum {
    using level = spdlog::level::level_enum;
    if (str == "trace") return level::trace;
    if (str == "info") return level::info;
    if (str == "warn") return level::warn;
    if (str == "err") return level::err;
    if (str == "critical") return level::critical;
    if (str == "off") return level::off;

    return level::debug;
}

Logger::Logger(const LoggingConfig& cfg) {
    const auto level = to_spdlog_level(cfg.level);
    spdlog::set_level(level);

    // prevents multi init of loggers since spdlog doesnt like that.
    static bool pool_init = false;
    if (!pool_init) {
        spdlog::init_thread_pool(8192, 1);
        pool_init = true;
    }

    // Create a distribution sink to hold multiple outputs
    auto dist_sink            = std::make_shared<spdlog::sinks::dist_sink_mt>();
    const std::string pattern = "[%Y-%m-%d %H:%M:%S] [%n] [%l] %v";

    // console logging
    if (cfg.console) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern(pattern);
        dist_sink->add_sink(console_sink);
    }

    // file logging
    if (!cfg.file_path.empty()) {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(cfg.file_path, true);
        file_sink->set_pattern(pattern);
        dist_sink->add_sink(file_sink);
    }

    auto create_chan = [&] (std::string name) {
        auto l = std::make_shared<spdlog::async_logger>(
            name,
            dist_sink,
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block
        );
        l->set_level(level);
        return l;
    };

    core     = create_chan("core");
    assets   = create_chan("assets");
    ecs      = create_chan("ecs");
    renderer = create_chan("renderer");
    ui       = create_chan("ui");

    spdlog::set_default_logger(core);
}

Logger::~Logger() {
    spdlog::shutdown();
}

} // namespace siren

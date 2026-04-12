module;

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/async.h>
#include <spdlog/cfg/env.h>
#include <fmt/format.h>
#include <source_location>

export module siren.log;

/// @todo Provide ways to create additional sinks?

namespace siren::log {

/** @brief The logging pattern used. */
constexpr std::string_view LogPattern = "%Y-%m-%dT%T.%f %^%l%$ [%t] [%s:%#] %v";

[[nodiscard]] auto logger_or_null_logger() -> spdlog::logger* {
    const auto logger = spdlog::default_logger_raw();
    if (logger) {
        return logger;
    }

    static auto null_logger = spdlog::create<spdlog::sinks::null_sink_mt>("null_sink");
    return null_logger.get();
}

auto log_inner(
    const std::source_location& source_location,
    const spdlog::level::level_enum lvl,
    const std::string_view fmt,
    const fmt::format_args args
) -> void {
    const spdlog::source_loc source{
        source_location.file_name(),
        static_cast<int>(source_location.line()),
        source_location.function_name(),
    };

    std::string msg = fmt::vformat(fmt, args);
    logger_or_null_logger()->log(source, lvl, "{}", msg);
}


export {
    using LogLevel = spdlog::level::level_enum;

    template <typename... Args>
    struct LogMessage {
        std::string_view fmt;
        std::source_location loc;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        consteval LogMessage(
            const char* s,
            const std::source_location loc = std::source_location::current()
        ) : fmt(s), loc(loc) { }
    };

    /** @brief Emits a log message to the trace level. */
    template <typename... Args>
    auto trace(
        LogMessage<std::type_identity_t<Args>...> msg,
        Args&&... args
    ) -> void {
        log_inner(msg.loc, spdlog::level::trace, msg.fmt, fmt::make_format_args(args...));
    }

    /** @brief Emits a log message to the debug level. */
    template <typename... Args>
    auto debug(
        LogMessage<std::type_identity_t<Args>...> msg,
        Args&&... args
    ) -> void {
        log_inner(msg.loc, spdlog::level::debug, msg.fmt, fmt::make_format_args(args...));
    }

    /** @brief Emits a log message to the info level. */
    template <typename... Args>
    auto info(
        LogMessage<std::type_identity_t<Args>...> msg,
        Args&&... args
    ) -> void {
        log_inner(msg.loc, spdlog::level::info, msg.fmt, fmt::make_format_args(args...));
    }

    /** @brief Emits a log message to the warn level. */
    template <typename... Args>
    auto warn(
        LogMessage<std::type_identity_t<Args>...> msg,
        Args&&... args
    ) -> void {
        log_inner(msg.loc, spdlog::level::warn, msg.fmt, fmt::make_format_args(args...));
    }

    /** @brief Emits a log message to the error level. */
    template <typename... Args>
    auto error(
        LogMessage<std::type_identity_t<Args>...> msg,
        Args&&... args
    ) -> void {
        log_inner(msg.loc, spdlog::level::err, msg.fmt, fmt::make_format_args(args...));
    }

    /** @brief Emits a log message to the critical level. */
    template <typename... Args>
    auto critical(
        LogMessage<std::type_identity_t<Args>...> msg,
        Args&&... args
    ) -> void {
        log_inner(msg.loc, spdlog::level::critical, msg.fmt, fmt::make_format_args(args...));
    }

    /** @brief Initializes the logging system. */
    auto init() -> void {
        spdlog::init_thread_pool(8192, 1);

        std::vector<spdlog::sink_ptr> sinks{
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
        };

        const auto logger = std::make_shared<spdlog::async_logger>(
            "siren",
            sinks.begin(),
            sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block
        );

        logger->set_pattern("[%T] [%^%l%$] %v");
        spdlog::flush_on(spdlog::level::err);
        spdlog::set_default_logger(logger);

        spdlog::cfg::load_env_levels("SIREN_LOG_LEVEL");
    }

    /** @brief Shuts down the logging system. */
    auto shutdown() -> void {
        spdlog::shutdown();
    }
}

} // namespace siren::logger

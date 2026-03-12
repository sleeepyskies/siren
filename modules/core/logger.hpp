#pragma once

#include <spdlog/spdlog.h>
#include "config.hpp"
#include "locator.hpp"


namespace siren
{
/** @brief Simple type alias for brevity. */
using logger_ptr = std::shared_ptr<spdlog::logger>;

/**
 * @brief Lists possible loggers in siren.
 * This enum corresponds to the loggers within @ref Logger.
 * This enum should be used as a template parameter for the
 * @ref WithLogger mixin. It is not required, but handles
 * fetching the correct logger for the class.
 */
enum class SystemLogger {
    Core,
    Assets,
    Ecs,
    Renderer,
    Ui,
};


namespace detail
{
auto logger_from_locator(SystemLogger type) -> logger_ptr;
} // namespace detail

/**
 * @class Logger
 * @brief Simple class containing loggers for all of Sirens core systems.
 * Caching loggers is fine. Access to the loggers should be done via the
 * @ref Locator.
 */
class Logger {
public:
    explicit Logger(const LoggingConfig& cfg);
    ~Logger();

    /// @brief The logger used for core subsystems.
    logger_ptr core;
    /// @brief The logger used internally by asset systems.
    logger_ptr assets;
    /// @brief The logger used internally by the ecs.
    logger_ptr ecs;
    /// @brief The logger used internally by the renderer.
    logger_ptr renderer;
    /// @brief The logger used internally for any ui.
    logger_ptr ui;
};

/**
 * @class WithLogger
 * @brief Simple mixin utility to fetch a logger for a class.
 *
 * The logger can be called via calling the log() function,
 * which returns a shared_ptr to the spdlog::logger
 *
 * The log() function is functions lazily, and it will not
 * assign a logger until the first time it is called.
 *
 * @tparam Log The @ref SystemLogger to use.
 */
template <SystemLogger Log>
class WithLogger {
protected:
    /**
     * @brief Returns a logger based on the template param
     * Log.
     * @return A pointer to a logger.
     */
    auto log() -> spdlog::logger* {
        if (!m_log) [[unlikely]] {
            m_log = detail::logger_from_locator(Log);
        }
        return m_log.get();
    }

private:
    /** @brief The underlying shared_ptr to the logger */
    logger_ptr m_log;
};

} // namespace siren

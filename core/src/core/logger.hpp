#pragma once

#include <memory>

#include "config.hpp"
#include "locator.hpp"

#include "spdlog/spdlog.h"


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
 */
template <SystemLogger Log>
class WithLogger {
protected:
    auto log() -> logger_ptr {
        if (!m_log) [[unlikely]] {
            const auto& log = core::Locator<Logger>::value();

            switch (Log) {
                case SystemLogger::Core: {
                    m_log = log.core;
                    break;
                }
                case SystemLogger::Assets: {
                    m_log = log.assets;
                    break;
                }
                case SystemLogger::Ecs: {
                    m_log = log.ecs;
                    break;
                }
                case SystemLogger::Renderer: {
                    m_log = log.renderer;
                    break;
                }
                case SystemLogger::Ui: {
                    m_log = log.ui;
                    break;
                }
            }
        }
        return m_log;
    }

private:
    logger_ptr m_log;
};

} // namespace siren

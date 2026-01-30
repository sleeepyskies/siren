#pragma once

#include <chrono>   // for timestamping
#include <cstring>  // for strrchr
#include <format>   // formatting various types and output
#include <print>    // for println
#include <mutex>    // thread safety
#include <string>   // strings :D

#define RESET "\033[0m"
#define GRAY "\033[90m"
#define BLUE "\033[34m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"


/// @brief slog Is a super lightweight logging library. It provides colored output, thread safety,
/// timestamped logging and variable log arguments.
namespace slog
{
/// @brief Enum representing the different log levels. Log level can be changed using setLevel()
enum class Level { Trace, Debug, Info, Warning, Error };

inline auto logLevel = Level::Debug;

class Logger {
public:
    template <typename... Args>
    static void trace(const std::string& fmt, const char* file, const int line, Args&&... args) {
        log(fmt, Level::Trace, file, line, GRAY, "TRC", std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void debug(const std::string& fmt, const char* file, const int line, Args&&... args) {
        log(fmt, Level::Debug, file, line, BLUE, "DBG", std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(const std::string& fmt, const char* file, const int line, Args&&... args) {
        log(fmt, Level::Info, file, line, GREEN, "NFO", std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warning(const std::string& fmt, const char* file, const int line, Args&&... args) {
        log(fmt, Level::Warning, file, line, YELLOW, "WRN", std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(const std::string& fmt, const char* file, const int line, Args&&... args) {
        log(fmt, Level::Error, file, line, RED, "ERR", std::forward<Args>(args)...);
    }

    static void set_level(const Level newLevel) {
        logLevel = newLevel;
    }

private:
    template <typename... Args>
    static void log(
        const std::string& fmt,
        const Level level,
        const char* file,
        const int line,
        const char* color,
        const char* levelStr,
        Args&&... args
    ) {
        if (logLevel > level) return;
        std::unique_lock lock(m_mutex);

        const auto now        = std::chrono::system_clock::now();
        const auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        const auto localTime  = std::chrono::current_zone()->to_local(nowSeconds);

        // shorten length of path to just filename and line
        const char* filename = strrchr(file, '/');
        if (!filename) filename = strrchr(file, '\\');
        filename = filename ? filename + 1 : file;

        std::println("{} [{} {}] {} {}:{} {}", color, levelStr, localTime, RESET, filename, line, args...);
    }

    inline static std::mutex m_mutex;
};
} // namespace slog

// Macros used for simplified logging, should be the preferred way to log.

#define trc(fmt, ...) slog::Logger::trace(fmt, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__)
#define dbg(fmt, ...) slog::Logger::debug(fmt, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__)
#define nfo(fmt, ...) slog::Logger::info(fmt, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__)
#define wrn(fmt, ...) slog::Logger::warning(fmt, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__)
#define err(fmt, ...) slog::Logger::error(fmt, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__)

#undef RESET
#undef GRAY
#undef BLUE
#undef RED
#undef YELLOW
#undef GREEN

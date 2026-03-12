export module siren.common:base;

export namespace siren
{
/// @brief Whether the engine compiles as s single threaded binary or not.
inline constexpr bool SINGLE_THREADED = false;

enum class Platform {
    Unix,
    Apple,
    Windows,
};

/// @brief The detected OS.
inline constexpr Platform PLATFORM =
    #if defined(unix) || defined(__unix) || defined(__unix__)
        Platform::Unix;
    #elif defined(__APPLE__)
        Platform::Apple;
    #elif defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
        Platform::Windows;
    #endif
} // namespace siren

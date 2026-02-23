#pragma once

#include "fkyaml_fwd.hpp"
#include "core/spch.hpp"


namespace siren
{

struct Config;

/**
 * @struct AssetsConfig
 * @brief Initial configuration for the assets in siren.
 */
struct AssetsConfig {
    /// @brief Root directory for asset loading
    std::string path = "./assets";
};

/**
 * @brief Window configuration used to initialize the @ref Window.
 */
struct WindowConfig {
    /// @brief The initial title of the window.
    std::string title = "Siren";
    /// @brief The initial width of the window
    u32 width = 1280;
    /// @brief The initial height of the window
    u32 height = 720;
    /// @brief If the window loads in fullscreen mode.
    bool fullscreen = false;
    /// @brief If vsync is enabled.
    bool vsync = true;
    /// @brief If the window has a title bar.
    bool decorated = true;
    /// @brief If the window is resizable.
    bool resizable = true;
    /// @brief If the window is transparent.
    bool transparent = false;
    /// @brief Separate flag for OpenGL as it requires extra glfw hints on init.
    bool is_opengl = true;
};

/**
 * @struct RendererConfig
 * @brief Initial configuration for the renderer of siren.
 */
struct RendererConfig {
    /// @brief The backend to pick on load. Can be either "auto" or "opengl".
    std::string backend = "auto";
};

/**
 * @struct LoggingConfig
 * @brief Configuration for the logging in siren.
 */
struct LoggingConfig {
    /// @brief The logging level of all loggers. Matches spdlog::level.
    std::string level = "debug";
    /// @brief Whether to log to the console o not
    bool console = true;
    /// @brief If and where to log to a physical file. Leave empty for nothing.
    std::string file_path = "";
};

/**
 * @brief Helper method used by the library fkYAML. Loads a config from a node.
 * @param node The fkYAML node to load from.
 * @param config The config to load into.
 */
inline auto from_node(const fkyaml::node& node, Config& config) -> void;

/**
 * @struct Config
 * @brief The central configuration for all global engine state.
 *
 * Mirrors the `config.siren` file. Also acts as a (very simple) clap.
 */
struct Config {

    /**
     * @brief Creates the @ref Config.
     *
     * Siren only accepts one cli arg, namely `--config path`.
     * The rest of the app configuration is done via
     * `config.siren`. By default, siren looks in the working
     * directory.
     *
     * @param argc The number of arguments passed into main.
     * @param argv The arguments passed into main.
     * @return A @ref Config.
     */
    static auto create(int argc, char* argv[]) -> Config;

    /// @brief The @ref WindowConfig.
    WindowConfig window_config{ };
    /// @brief The @ref RendererConfig.
    RendererConfig renderer_config{ };
    /// @brief The @ref LoggingConfig.
    LoggingConfig logging_config{ };
    /// @brief The @ref AssetsConfig.
    AssetsConfig assets_config{ };
};

} // namespace siren

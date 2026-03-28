module;

#include <string>

export module siren.window:config;

import siren.common;

namespace siren::window {

/**
 * @brief Initialization data of the @ref WindowPlugin.
 */
export struct WindowConfig {
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

} // namespace siren::window

#pragma once

#include "core/spch.hpp"
#include "input/input_codes.hpp"


namespace siren::core
{

enum class WindowMode {
    Windowed,
    Minimized,
    Maximized,
};

/**
 * @brief Simple struct containing settings for initializing the @ref Window.
 */
struct WindowSettings {
    /// @brief The title of the window.
    std::string title;
    /// @brief The initial width of the @ref Window.
    u32 width;
    /// @brief The initial height of the @ref Window.
    u32 height;
    /// @brief If the window should have a title bar or not.
    bool decorated;
    /// @brief If the window should be resizable.
    bool resizable;
    /// @brief If the window should have v-sync enabled or not.
    bool vsync;
    /// @brief If the window be transparent.
    bool transparent;
};

class Window {
public:
    explicit Window(const WindowSettings& settings);
    ~Window();

    Window(const Window&) = delete;
    Window(Window&& other) noexcept;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&& other) noexcept;

    [[nodiscard]] auto handle() const noexcept -> void*;
    [[nodiscard]] auto width() const noexcept -> u32;
    [[nodiscard]] auto height() const noexcept -> u32;
    [[nodiscard]] auto size() const noexcept -> glm::vec2;
    [[nodiscard]] auto title() const noexcept -> std::string_view;
    [[nodiscard]] auto minimized() const noexcept -> bool;
    [[nodiscard]] auto maximized() const noexcept -> bool;
    [[nodiscard]] auto should_close() const noexcept -> bool;

    auto poll_events() const -> void;
    auto set_title(const std::string& title) -> void;
    auto set_maximized(bool value) -> void;
    auto set_minimized(bool value) -> void;
    auto set_vsync(bool value) -> void;

private:
    std::shared_ptr<spdlog::logger> m_logger;
};

/**
 * @brief The WindowModule manages the Window.
 * Currently, we only support one native Siren Window.
 */
class WindowModule {
public:
    WindowModule();

    /// @brief Polls the window for events.
    void poll_events() const;
    /// @brief Returns true if the window should be closed.
    bool should_close() const;
    /// @brief Presents the back buffer to the screen.
    void swap_buffers() const;
    /// @brief Returns the size of this window.
    glm::ivec2 size() const;
    /// @brief Sets the title of the window.
    void set_title(const std::string& title) const;
    /// @brief Enables or disables vSync.
    void set_vsync(bool value) const;
    /// @brief Returns the mouse mode of the current window.
    CursorMode mouse_mode() const;
    /// @brief Sets the mouse mode of the current window.
    void set_mouse_mode(CursorMode mode) const;
    /// @brief Returns the handle of the underlying window.
    void* handle() const;
};
} // namespace siren::core

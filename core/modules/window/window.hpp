#pragma once

#include "core/spch.hpp"
#include "sync/mutex.hpp"

struct GLFWwindow;


namespace siren::core
{

/**
 * @brief Represents a state a @ref Window can be in. Iff the window exists,
 * it must be in one of these states.
 */
enum class WindowMode {
    /** @brief Standard window mode. */
    Normal,
    /** @brief The window is minimized, meaning not visible and on the taskbar. */
    Minimized,
    /** @brief The window is maximized. Not to be confused with fullscreen. */
    Maximized,
    /** @brief The window is fullscreen. GPU has fully taken over the monitor. */
    Fullscreen
};

/**
 * @class Window
 * @brief A thread safe representation of a native window.
 * This class handles window lifecycle and input event translation via GLFW.
 * @note Atm, this acts as a semi WindowManager and single Window instance.
 * Since siren does not make use of multiple windows at the moment, this
 * is however fine.
 */
class Window : WithLogger<SystemLogger::Core> {
public:
    explicit Window(const WindowConfig& cfg);
    ~Window();

    Window(const Window&)                      = delete;
    Window(Window&& other)                     = delete;
    Window& operator=(const Window&)           = delete;
    Window& operator=(Window&& other) noexcept = delete;

    /** @brief Returns a raw handle to the underlying GLFW window */
    [[nodiscard]] auto handle() const noexcept -> void*;
    /** @brief Returns the current width of the window. */
    [[nodiscard]] auto width() const noexcept -> u32;
    /** @brief Returns the current height of the window. */
    [[nodiscard]] auto height() const noexcept -> u32;
    /** @brief Returns the current size of the window. */
    [[nodiscard]] auto size() const noexcept -> glm::uvec2;
    /** @brief Returns the current position of the window. */
    [[nodiscard]] auto position() const noexcept -> glm::ivec2;
    /** @brief Returns the current title of the window. */
    [[nodiscard]] auto title() const noexcept -> std::string_view;
    /** @brief Checks whether the window is currently minimized. */
    [[nodiscard]] auto is_minimized() const noexcept -> bool;
    /** @brief Checks whether the window is currently maximized. */
    [[nodiscard]] auto is_maximized() const noexcept -> bool;
    /** @brief Checks whether the window is currently fullscreen. */
    [[nodiscard]] auto is_fullscreen() const noexcept -> bool;
    /** @brief Checks whether the window should close. */
    [[nodiscard]] auto should_close() const noexcept -> bool;

    /**
     * @brief Sets the title of the window.
     * @param title The new window title.
     */
    auto set_title(const std::string& title) const -> void;
    /** @brief Minimizes the window. */
    auto minimize() const -> void;
    /** @brief Maximizes the window. */
    auto maximize() const -> void;
    /**
     * @brief Sets the fullscreen status of the window.
     * @param val Whether to enable or disable fullscreen.
     */
    auto set_fullscreen(bool val) const -> void;
    /**
     * @brief Sets the size of the window.
     * @param size The new size of the window.
     */
    auto set_size(glm::uvec2 size) const -> void;
    /**
     * @brief Sets the position of the window.
     * @param position The new position of the window.
     */
    auto set_position(glm::ivec2 position) const -> void;

private:
    friend class App;

    /**
     * @brief Processes the internal request queue and polls for any OS events
     * @warning This must only be called from the main thread!!!
     */
    auto poll_events() const -> void;

    /** @brief Inner helper method to link glfw callbacks to the siren @ref EventBus. */
    auto register_event_emitters() const -> void;
    /** @brief Inner helper method to react to any siren events. */
    auto register_event_handlers() const -> void;

    /** @brief Callback function type used internally to defer execution of certain requests. */
    using WindowRequest = std::function<void()>;

    GLFWwindow* m_window;
    mutable std::atomic<WindowMode> m_window_mode;
    Mutex<glm::uvec2> m_size;
    Mutex<glm::ivec2> m_position;
    Mutex<std::string> m_title;
    Mutex<std::vector<WindowRequest>> m_requests;
    SwapChain m_swapchain;
};

} // namespace siren::core

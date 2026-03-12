module;

#include <atomic>
#include <functional>
#include <string>
#include <GLFW/glfw3.h>
#include "assert.hpp"

export module siren.window:window;

import siren.sync;
import siren.common;
import siren.math;
import siren.log;
import siren.signal;

import :config;
import :signals;

namespace siren::window {

/**
 * @brief Represents a state a @ref Window can be in. Iff the window exists,
 * it must be in one of these states.
 */
export enum class WindowMode {
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
export class Window {
public:
    explicit Window(const WindowConfig& cfg, signal::SignalBus& signal_bus);
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

    /**
     * @brief Processes the internal request queue and polls for any OS events
     * @warning This must only be called from the main thread!!!
     */
    auto poll_events() const -> void;

private:
    /** @brief Inner helper method to link glfw callbacks to the siren @ref EventBus. */
    auto register_event_emitters(signal::SignalBus& signal_bus) const -> void;
    /** @brief Inner helper method to react to any siren events. */
    auto register_event_handlers() const -> void;

    /** @brief Callback function type used internally to defer execution of certain requests. */
    using WindowRequest = std::function<void()>;

    GLFWwindow* m_window;
    mutable std::atomic<WindowMode> m_window_mode;
    sync::Mutex<glm::uvec2> m_size;
    sync::Mutex<glm::ivec2> m_position;
    sync::Mutex<std::string> m_title;
    sync::Mutex<std::vector<WindowRequest>> m_requests;
    SwapChain m_swapchain;
};

/// Helper to retrieve a siren Window from a glfw Window User Pointer.
constexpr auto to_siren_window(GLFWwindow* window) -> Window& {
    return *static_cast<Window*>(glfwGetWindowUserPointer(window));
}

constexpr auto to_siren_mods(const i32 mods) -> Modifiers {
    return Modifiers{
        .shift = (bool)(mods & GLFW_MOD_SHIFT),
        .control = (bool)(mods & GLFW_MOD_CONTROL),
        .alt = (bool)(mods & GLFW_MOD_ALT),
        .super = (bool)(mods & GLFW_MOD_SUPER),
        .caps_lock = (bool)(mods & GLFW_MOD_CAPS_LOCK),
        .num_lock = (bool)(mods & GLFW_MOD_NUM_LOCK)
    };
}

Window::Window(const WindowConfig& cfg, signal::SignalBus& signal_bus) {
    glfwSetErrorCallback(glfw_error_callback);
    SIREN_ASSERT(glfwInit(), "Failed to initialize GLFW");

    GLFWmonitor* monitor = nullptr;
    if (cfg.fullscreen) {
        monitor = glfwGetPrimaryMonitor();
    }

    if (cfg.is_opengl) {
        // opengl requires special hints since it has a context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    } else {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    if (cfg.decorated) {
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

    if (cfg.resizable) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    if (cfg.transparent) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
    }

    m_window = glfwCreateWindow(cfg.width, cfg.height, cfg.title.c_str(), monitor, nullptr);
    SIREN_ASSERT(m_window, "Failed to create GLFW window");

    // don't set vsync here, render thread should do this since its context dependent

    glfwSetWindowUserPointer(m_window, this);

    register_event_emitters(signal_bus);
    register_event_handlers(signal_bus);

    log::info("Window created successfully: {}x{}", cfg.width, cfg.height);
    glfwMakeContextCurrent(nullptr);
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    // todo: once many windows are supported this should go in the WindowManager or something
    glfwTerminate();
}

auto Window::handle() const noexcept -> void* {
    return m_window;
}

auto Window::width() const noexcept -> u32 {
    return size().x;
}

auto Window::height() const noexcept -> u32 {
    return size().y;
}

auto Window::size() const noexcept -> glm::uvec2 {
    return m_size.get();
}

auto Window::position() const noexcept -> glm::ivec2 {
    return m_position.get();
}

auto Window::title() const noexcept -> std::string_view {
    return m_title.get();
}

auto Window::is_minimized() const noexcept -> bool {
    return m_window_mode.load() == WindowMode::Minimized;
}

auto Window::is_maximized() const noexcept -> bool {
    return m_window_mode.load() == WindowMode::Maximized;
}
auto Window::is_fullscreen() const noexcept -> bool {
    return m_window_mode.load() == WindowMode::Fullscreen;
}

auto Window::should_close() const noexcept -> bool {
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

auto Window::set_title(const std::string& title) const -> void {
    m_requests.lock()->emplace_back([this, title] { glfwSetWindowTitle(m_window, title.c_str()); });
}

auto Window::minimize() const -> void {
    m_requests.lock()->emplace_back([this] { glfwIconifyWindow(m_window); });
}

auto Window::maximize() const -> void {
    m_requests.lock()->emplace_back([this] { glfwMaximizeWindow(m_window); });
}

auto Window::set_fullscreen(const bool val) const -> void {
    // setting to same, so skip
    if ((val && m_window_mode == WindowMode::Fullscreen) || !val && m_window_mode != WindowMode::Fullscreen) {
        return;
    }

    // store vals so we remember what to go back to on fs exit
    static i32 cached_x, cached_y, cached_w, cached_h;
    if (val) {
        cached_x = position().x;
        cached_y = position().y;
        cached_w = size().x;
        cached_h = size().y;
    }

    m_requests.lock()->emplace_back(
        [this, val] {
            if (val) {
                const auto monitor = glfwGetPrimaryMonitor();
                const auto mode    = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                this->m_window_mode = WindowMode::Fullscreen;
            } else {
                //
                glfwSetWindowMonitor(m_window, nullptr, cached_x, cached_y, cached_w, cached_h, 0);
            }
        }
    );
}

auto Window::set_size(glm::uvec2 size) const -> void {
    m_requests.lock()->emplace_back([this, size] { glfwSetWindowSize(m_window, size.x, size.y); });
}

auto Window::set_position(glm::ivec2 position) const -> void {
    m_requests.lock()->emplace_back([this, position] { glfwSetWindowPos(m_window, position.x, position.y); });
}

auto Window::poll_events() const -> void {
    // first, we handle any requests that were made in the previous frame
    std::vector<WindowRequest> requests;
    {
        auto guard = m_requests.lock();
        requests   = std::move(*guard);
        guard->clear();
    }

    for (const auto& request : requests) {
        request();
    }

    // poll events
    glfwPollEvents();
}

auto Window::register_event_emitters(signal::SignalBus& signal_bus) const -> void {
    /*
    glfwSetMonitorCallback([] (GLFWmonitor* monitor, i32 event) { });
    glfwSetJoystickCallback([] (i32 jid, i32 event) { });
    */

    // glfwSetFramebufferSizeCallback(window, [] (GLFWwindow* w, i32 width, i32 height) { });
    // glfwSetWindowContentScaleCallback(window, [] (GLFWwindow* w, f32 xscale, f32 yscale) { });
    // glfwSetWindowRefreshCallback(window, [] (GLFWwindow* w) { });
    // glfwSetWindowFocusCallback(window, [] (GLFWwindow* w, i32 focused) { });

    glfwSetWindowPosCallback(
        m_window,
        [] (GLFWwindow* w, const i32 x, const i32 y) {
            const auto& self = *static_cast<Window*>(glfwGetWindowUserPointer(w));
            self.set_position(glm::vec2{ x, y });
            signals.emit<WindowMoveSignal>(glm::vec2{ x, y });
        }
    );

    glfwSetWindowSizeCallback(
        m_window,
        [] (GLFWwindow* w, const i32 width, const i32 height) {
            auto& signals = *static_cast<signal::SignalBus*>(glfwGetWindowUserPointer(w));
            signals.emit<WindowResizeSignal>(glm::ivec2{ width, height });
        }
    );

    glfwSetWindowCloseCallback(
        m_window,
        [] (GLFWwindow* w) {
            auto& signals = *static_cast<signal::SignalBus*>(glfwGetWindowUserPointer(w));
            signals.emit<WindowClosedSignal>();
        }
    );

    glfwSetWindowIconifyCallback(
        m_window,
        [] (GLFWwindow* w, const i32 iconified) {
            if (iconified == GLFW_TRUE) {
                auto& signals = *static_cast<signal::SignalBus*>(glfwGetWindowUserPointer(w));
                signals.emit<WindowMinimizedSignal>();
            }
        }
    );

    glfwSetWindowMaximizeCallback(
        m_window,
        [] (GLFWwindow* w, const i32 iconified) {
            if (iconified == GLFW_TRUE) {
                auto& signals = *static_cast<signal::SignalBus*>(glfwGetWindowUserPointer(w));
                signals.emit<WindowMaximizedSignal>();
            }
        }
    );

    // glfwSetDropCallback(window, [] (GLFWwindow* w, i32 count, const char** paths) { });
    // glfwSetCursorEnterCallback(window, [] (GLFWwindow* w, i32 entered) { });
    // glfwSetCharCallback(window, [] (GLFWwindow* w, u32 codepoint) { });

    glfwSetKeyCallback(
        m_window,
        [] (GLFWwindow* w, const i32 key, i32 scancode, const i32 action, const i32 mods) {
            if (action == GLFW_PRESS) {
                const auto siren_mods = to_siren_mods(mods);
                signal_bus.post<KeyboardButtonPressedEvent>(platform::from_glfw_key(key), siren_mods);
            } else if (action == GLFW_RELEASE) {
                signal_bus.post<KeyboardButtonReleasedEvent>(platform::from_glfw_key(key));
            }
        }
    );

    glfwSetMouseButtonCallback(
        m_window,
        [] (GLFWwindow* w, const i32 button, const i32 action, const i32 mods) {
            if (action == GLFW_PRESS) {
                const auto siren_mods = to_siren_mods(mods);
                signal_bus.post<MouseButtonPressedEvent>(platform::from_glfw_mouse(button), siren_mods);
            } else if (action == GLFW_RELEASE) {
                signal_bus.post<MouseButtonReleasedEvent>(platform::from_glfw_mouse(button));
            }
        }
    );

    glfwSetCursorPosCallback(
        m_window,
        [] (GLFWwindow* w, const double xpos, const double ypos) {
            signal_bus.post<MouseMotionEvent>(glm::vec2{ xpos, ypos });
        }
    );

    glfwSetScrollCallback(
        m_window,
        [] (GLFWwindow* w, const double xoffset, const double yoffset) {
            signal_bus.post<ScrollEvent>(glm::vec2{ xoffset, yoffset });
        }
    );
}

auto Window::register_event_handlers(signal::SignalBus& signal_bus) const -> void {
    signal_bus.subscribe<WindowMoveSignal>(
        [this] (const WindowMoveEvent& event) {
            m_position.set(event.position);
        }
    );

    signal_bus.subscribe<WindowResizeEvent>(
        [this] (const WindowResizeEvent& event) {
            m_size.set(event.size);
        }
    );

    signal_bus.subscribe<WindowMaximizedEvent>(
        [this] (auto&&) {
            this->m_window_mode.store(WindowMode::Maximized);
        }
    );

    signal_bus.subscribe<WindowMinimizedEvent>(
        [this] (auto&&) {
            this->m_window_mode.store(WindowMode::Minimized);
        }
    );
}

} // namespace siren::window

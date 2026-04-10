module;

#include <atomic>
#include <functional>
#include <string>
#include <GLFW/glfw3.h>
#include <libassert/assert.hpp>

export module siren.window:window;

import siren.sync;
import siren.common;
import siren.math;
import siren.log;

import :config;

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
    explicit Window(const WindowConfig& cfg);
    ~Window();

    Window(const Window&)                      = delete;
    Window(Window&& other)                     = delete;
    Window& operator=(const Window&)           = delete;
    Window& operator=(Window&& other) noexcept = delete;

    /** @brief Returns a raw handle to the underlying GLFW window */
    [[nodiscard]] auto handle() const noexcept -> void*;
    /** @brief Returns a GLFW handle to the underlying window */
    [[nodiscard]] auto glfw_handle() const noexcept -> GLFWwindow*;
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

    /** @brief Sets the title of the window. */
    auto set_title(const std::string& title) const -> void;
    /** @brief Closes the window. @warning May cause the application to end. */
    auto close() noexcept -> void;
    /** @brief Minimizes the window. */
    auto minimize() const -> void;
    /** @brief Maximizes the window. */
    auto maximize() const -> void;
    /** @brief Sets the fullscreen status of the window.*/
    auto set_fullscreen(bool val) const -> void;
    /** @brief Sets the size of the window. */
    auto set_size(glm::uvec2 size) const -> void;
    /** @brief Sets the position of the window. */
    auto set_position(glm::ivec2 position) const -> void;

    /**
     * @brief Processes the internal request queue and polls for any OS events
     * @warning This must only be called from the main thread!!!
     */
    auto poll_events() const -> void;

private:
    friend class WindowPlugin;

    /** @brief Callback function type used internally to defer execution of certain requests. */
    using WindowRequest = std::function<void()>;

    GLFWwindow* m_window;
    std::atomic_bool m_should_close = false;
    mutable std::atomic<WindowMode> m_window_mode;
    sync::Mutex<glm::uvec2> m_size;
    sync::Mutex<glm::ivec2> m_position;
    sync::Mutex<std::string> m_title;
    sync::Mutex<std::vector<WindowRequest>> m_requests;
    // SwapChain m_swapchain;
};

Window::Window(const WindowConfig& cfg) {
    ASSERT();
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

auto Window::glfw_handle() const noexcept -> GLFWwindow* {
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
    return m_window == nullptr || m_should_close.load() || glfwWindowShouldClose(m_window);
}

auto Window::set_title(const std::string& title) const -> void {
    m_requests.lock()->emplace_back([this, title] { glfwSetWindowTitle(m_window, title.c_str()); });
}

auto Window::close() noexcept -> void {
    m_should_close.store(true, std::memory_order_relaxed);
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
    const std::vector<WindowRequest> requests = m_requests.run_scoped(
        [] (sync::UniqueGuard<std::vector<WindowRequest>> guard) {
            return std::move(*guard);
        }
    );

    for (const auto& request : requests) {
        request();
    }

    // poll events
    glfwPollEvents();
}

} // namespace siren::window

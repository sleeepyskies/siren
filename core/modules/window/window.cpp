#include "window.hpp"

#include <GLFW/glfw3.h>

#include "core/event_bus.hpp"
#include "core/events.hpp"

#include "platform/opengl/debug.hpp"
#include "platform/glfw/key_mappings.hpp"


namespace siren::core
{
/// Helper to retrieve a siren Window from a glfw Window User Pointer.
static auto to_siren_window(GLFWwindow* window) -> Window& {
    return *static_cast<Window*>(glfwGetWindowUserPointer(window));
}

/// Simple helper method to retrieve the EventBus
static auto event_bus() -> EventBus& { return Locator<EventBus>::value(); }

static auto to_siren_mods(const i32 mods) -> Modifiers {
    return Modifiers{
        .shift = (bool)(mods & GLFW_MOD_SHIFT),
        .control = (bool)(mods & GLFW_MOD_CONTROL),
        .alt = (bool)(mods & GLFW_MOD_ALT),
        .super = (bool)(mods & GLFW_MOD_SUPER),
        .caps_lock = (bool)(mods & GLFW_MOD_CAPS_LOCK),
        .num_lock = (bool)(mods & GLFW_MOD_NUM_LOCK)
    };
}

Window::Window(const WindowConfig& cfg) {
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

    register_event_emitters();
    register_event_handlers();

    log()->info("Window created successfully: {}x{}", cfg.width, cfg.height);
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
    if ((val && m_window_mode == WindowMode::Fullscreen) || !val && m_window_mode != WindowMode::Fullscreen) { return; }

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

    ranges::for_each(requests, lambda { it(); });

    // poll events
    glfwPollEvents();
}

auto Window::register_event_emitters() const -> void {
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
            event_bus().post<WindowMoveEvent>(glm::vec2{ x, y });
        }
    );

    glfwSetWindowSizeCallback(
        m_window,
        [] (GLFWwindow*, const i32 width, const i32 height) {
            event_bus().post<WindowResizeEvent>(glm::ivec2{ width, height });
        }
    );

    glfwSetWindowCloseCallback(
        m_window,
        [] (GLFWwindow*) {
            event_bus().post<WindowCloseEvent>();
        }
    );

    glfwSetWindowIconifyCallback(
        m_window,
        [] (GLFWwindow*, const i32 iconified) {
            if (iconified == GLFW_TRUE) {
                event_bus().post<WindowMinimizedEvent>();
            }
        }
    );

    glfwSetWindowMaximizeCallback(
        m_window,
        [] (GLFWwindow*, const i32 iconified) {
            if (iconified == GLFW_TRUE) {
                event_bus().post<WindowMaximizedEvent>();
            }
        }
    );

    // glfwSetDropCallback(window, [] (GLFWwindow* w, i32 count, const char** paths) { });
    // glfwSetCursorEnterCallback(window, [] (GLFWwindow* w, i32 entered) { });
    // glfwSetCharCallback(window, [] (GLFWwindow* w, u32 codepoint) { });

    glfwSetKeyCallback(
        m_window,
        [] (GLFWwindow*, const i32 key, i32 scancode, const i32 action, const i32 mods) {
            if (action == GLFW_PRESS) {
                const auto siren_mods = to_siren_mods(mods);
                event_bus().post<KeyboardButtonPressedEvent>(platform::from_glfw_key(key), siren_mods);
            } else if (action == GLFW_RELEASE) {
                event_bus().post<KeyboardButtonReleasedEvent>(platform::from_glfw_key(key));
            }
        }
    );

    glfwSetMouseButtonCallback(
        m_window,
        [] (GLFWwindow* w, const i32 button, const i32 action, const i32 mods) {
            if (action == GLFW_PRESS) {
                const auto siren_mods = to_siren_mods(mods);
                event_bus().post<MouseButtonPressedEvent>(platform::from_glfw_mouse(button), siren_mods);
            } else if (action == GLFW_RELEASE) {
                event_bus().post<MouseButtonReleasedEvent>(platform::from_glfw_mouse(button));
            }
        }
    );

    glfwSetCursorPosCallback(
        m_window,
        [] (GLFWwindow* w, const double xpos, const double ypos) {
            event_bus().post<MouseMotionEvent>(glm::vec2{ xpos, ypos });
        }
    );

    glfwSetScrollCallback(
        m_window,
        [] (GLFWwindow* w, const double xoffset, const double yoffset) {
            event_bus().post<ScrollEvent>(glm::vec2{ xoffset, yoffset });
        }
    );
}

auto Window::register_event_handlers() const -> void {
    event_bus().subscribe<WindowMoveEvent>(
        [this] (const WindowMoveEvent& event) {
            m_position.set(event.position);
        }
    );

    event_bus().subscribe<WindowResizeEvent>(
        [this] (const WindowResizeEvent& event) {
            m_size.set(event.size);
        }
    );

    event_bus().subscribe<WindowMaximizedEvent>(
        [this] (auto&&) {
            this->m_window_mode.store(WindowMode::Maximized);
        }
    );

    event_bus().subscribe<WindowMinimizedEvent>(
        [this] (auto&&) {
            this->m_window_mode.store(WindowMode::Minimized);
        }
    );
}

} // namespace siren::core

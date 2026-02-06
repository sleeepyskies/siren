#include "WindowsWindow.hpp"

#include "WindowsUtils.hpp"

#include "core/Debug.hpp"
#include "core/Locator.hpp"

#include "events/Events.hpp"


namespace siren::platform
{
WindowsWindow::WindowsWindow(const Properties& properties) : Window(properties) {
    // Make OpenGL Context class for this
    glfwSetErrorCallback(core::glfw_error_callback);
    glfwInit();

    // specify version and features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); todo: remove once custom title bar exists, should be in editor tho actually.

    m_window = glfwCreateWindow(
        m_properties.width,
        m_properties.height,
        m_properties.title.c_str(),
        nullptr,
        nullptr
    );
    if (!m_window) {
        err("Could not create window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(m_properties.vSyncEnabled);

    // load opengl functions
    // ReSharper disable once CppCStyleCast
    const int glVersion = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!glVersion) {
        err("OpenGl could not load correctly");
        glfwTerminate();
    }

    // TODO: put in OpenGLContext class
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    nfo("Loaded OpenGl version {}.{}", major, minor);

    // init OpenGL debug logging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(core::opengl_error_callback, nullptr);

    setupCallbacks();
}

WindowsWindow::~WindowsWindow() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

void WindowsWindow::poll_events() {
    glfwPollEvents();
}

bool WindowsWindow::should_close() const {
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

void WindowsWindow::swap_buffers() const {
    glfwSwapBuffers(m_window);
}

glm::ivec2 WindowsWindow::size() const {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return { w, h };
}

void WindowsWindow::set_title(const std::string& title) {
    m_properties.title = title;
    glfwSetWindowTitle(m_window, m_properties.title.c_str());
}

void WindowsWindow::set_vsync(const bool value) {
    m_properties.vSyncEnabled = value;
    glfwSwapInterval(m_properties.vSyncEnabled);
    dbg("Vsync set to {}", value);
}

core::MouseMode WindowsWindow::mouse_mode() const {
    return fromGLFWMouseMode(glfwGetInputMode(m_window, GLFW_CURSOR));
}

void WindowsWindow::set_mouse_mode(const core::MouseMode mode) {
    glfwSetInputMode(m_window, GLFW_CURSOR, toGLFWMouseMode(mode));
}

void* WindowsWindow::handle() {
    return m_window;
}

void WindowsWindow::setupCallbacks() const {
    glfwSetWindowSizeCallback(
        m_window,
        [] (GLFWwindow*, i32 w, i32 h) {
            core::Locator<core::EventBus>::locate().Emit<core::WindowResizeEvent>(w, h);
        }
    );

    glfwSetScrollCallback(
        m_window,
        [] (GLFWwindow*, double xOffset, double yOffset) {
            core::App::get().GetEventBus().Post<core::ScrollEvent>(xOffset, yOffset);
        }
    );

    glfwSetKeyCallback(
        m_window,
        [] (GLFWwindow*, const int key, int, int const action, int) {
            if (action == GLFW_PRESS) {
                core::App::get().GetEventBus().Post<core::KeyPressedEvent>(fromGLFW(key));
            } else if (action == GLFW_RELEASE) {
                core::App::get().GetEventBus().Post<core::KeyReleasedEvent>(fromGLFW(key));
            }
        }
    );

    glfwSetMouseButtonCallback(
        m_window,
        [] (GLFWwindow*, const int button, const int action, int) {
            if (action == GLFW_PRESS) {
                core::App::get().GetEventBus().Post<core::MouseKeyPressedEvent>(fromGLFWMouse(button));
            } else if (action == GLFW_RELEASE) {
                core::App::get().GetEventBus().Post<core::MouseKeyReleasedEvent>(fromGLFWMouse(button));
            }
        }
    );

    glfwSetCursorPosCallback(
        m_window,
        [] (GLFWwindow*, double xPos, double yPos) {
            core::App::get().GetEventBus().Post<core::MouseMovedEvent>(xPos, yPos);
        }
    );

    glfwSetWindowCloseCallback(
        m_window,
        [] (GLFWwindow*) {
            core::App::get().GetEventBus().Emit<core::AppCloseEvent>();
        }
    );

    // todo: more callbacks here ig, but have no use for yet.
}
} // namespace siren::platform

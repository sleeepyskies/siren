#include "Window.hpp"

#include "Debug.hpp"
#include "event/KeyEvent.hpp"
#include "event/MouseKeyEvent.hpp"
#include "event/WindowEvent.hpp"

namespace siren::core
{

Window::Window(const Properties& properties) : m_properties(properties)
{
}

void Window::setCallbacks() const
{
    // on actual window resize
    glfwSetWindowSizeCallback(handle(), [](GLFWwindow* win, const int w, const int h) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        glViewport(0, 0, w, h);

        window->m_properties.width  = w;
        window->m_properties.height = h;

        event::WindowResizeEvent e{ w, h };
        window->m_eventCallback(e);

        trc("{}", e.toString());
    });

    // on window close
    glfwSetWindowCloseCallback(handle(), [](GLFWwindow* win) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        event::WindowCloseEvent e{};
        window->m_eventCallback(e);

        trc("{}", e.toString());
    });

    // on any keyboard key action
    glfwSetKeyCallback(handle(), [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        switch (action) {
            case GLFW_PRESS: {
                event::KeyPressEvent e{ static_cast<KeyCode>(key) };
                window->m_eventCallback(e);
                trc("{}", e.toString());
                break;
            }
            case GLFW_REPEAT: {
                event::KeyPressEvent e{ static_cast<KeyCode>(key), true };
                window->m_eventCallback(e);
                // don't log repeats to avoid spam
                break;
            }
            case GLFW_RELEASE: {
                event::KeyReleaseEvent e{ static_cast<KeyCode>(key) };
                window->m_eventCallback(e);
                trc("{}", e.toString());
                break;
            }
            default: {
                SirenAssert(false, "Encountered invalid KeyEvent from GLFW");
            }
        }
    });

    // any mouse button inputs
    glfwSetMouseButtonCallback(handle(), [](GLFWwindow* win, int button, int action, int mods) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        switch (action) {
            case GLFW_PRESS: {
                event::MousePressEvent e{ static_cast<MouseCode>(button) };
                window->m_eventCallback(e);
                trc("{}", e.toString());
                break;
            }
            case GLFW_RELEASE: {
                event::MouseReleaseEvent e{ static_cast<MouseCode>(button) };
                window->m_eventCallback(e);
                trc("{}", e.toString());
                break;
            }
            default: {
                SirenAssert(false, "Encountered invalid MouseKeyEvent from GLFW");
            }
        }
    });

    // any mouse movement
    glfwSetCursorPosCallback(handle(), [](GLFWwindow* win, const double xpos, const double ypos) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        event::MouseMoveEvent e{ static_cast<float>(xpos), static_cast<float>(ypos) };
        window->m_eventCallback(e);

        // too frequent even for trace
        // trc("{}", e.toString());
    });
}

void Window::init()
{
    // Make OpenGL Context class for this
    glfwSetErrorCallback(GLFWErrorCallback);
    glfwInit();

    // specify version and features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(
        m_properties.width, m_properties.height, m_properties.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        err("Could not create window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSwapInterval(m_properties.vSyncEnabled);

    glfwSetWindowUserPointer(handle(), this);

    // load opengl functions
    const int glVersion = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
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
    glDebugMessageCallback(OpenGLErrorCallback, nullptr);

    setCallbacks();
}

void Window::destroy()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::setEventCallback(const event::EventCallback& callback)
{
    m_eventCallback = callback;
}

bool Window::shouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

void Window::close()
{
    glfwSetWindowShouldClose(m_window, true);
}

void Window::setTitle(const std::string& title)
{
    m_properties.title = title;
    glfwSetWindowTitle(m_window, m_properties.title.c_str());
}

void Window::setVsync(const bool value)
{
    m_properties.vSyncEnabled = value;
    glfwSwapInterval(m_properties.vSyncEnabled);
    dbg("Vsync set to {}", value);
}

// TODO: move this into renderer? or some renderer context specific class
void Window::clearBuffers() const
{
    const glm::vec4 color = m_properties.backgroundColor;
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

GLFWwindow* Window::handle() const
{
    return m_window;
}

glm::ivec2 Window::getSize() const
{
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return { w, h };
}

} // namespace siren::core

#include "Window.hpp"

#include "events/KeyEvent.hpp"
#include "events/MouseKeyEvent.hpp"
#include "events/WindowEvent.hpp"

namespace core
{

Window::Window(const Properties& properties = Properties()) : m_properties(properties)
{
}

void Window::setCallbacks() const
{
    // on actual window resize
    glfwSetWindowSizeCallback(handle(), [](GLFWwindow* win, const int w, const int h) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        glViewport(0, 0, w, h);

        window->m_properties.width  = w;
        window->m_properties.height = w;

        WindowResizeEvent e{ w, h };
        window->m_eventCallback(e);

        trc("{}", e.toString());
    });

    // on window close
    glfwSetWindowCloseCallback(handle(), [](GLFWwindow* win) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        WindowCloseEvent e{};
        window->m_eventCallback(e);

        trc("{}", e.toString());
    });

    // on any keyboard key action
    glfwSetKeyCallback(handle(), [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        const auto window = static_cast<Window*>(glfwGetWindowUserPointer(win));

        switch (action) {
        case GLFW_PRESS: {
            KeyPressEvent e{ static_cast<KeyCode>(key) };
            window->m_eventCallback(e);
            trc("{}", e.toString());
            break;
        }
        case GLFW_REPEAT: {
            KeyPressEvent e{ static_cast<KeyCode>(key), true };
            window->m_eventCallback(e);
            trc("{}", e.toString());
            break;
        }
        case GLFW_RELEASE: {
            KeyReleaseEvent e{ static_cast<KeyCode>(key) };
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
            MousePressEvent e{ static_cast<MouseCode>(button) };
            window->m_eventCallback(e);
            trc("{}", e.toString());
            break;
        }
        case GLFW_RELEASE: {
            MouseReleaseEvent e{ static_cast<MouseCode>(button) };
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

        MouseMoveEvent e{ static_cast<float>(xpos), static_cast<float>(ypos) };
        window->m_eventCallback(e);

        // too frequent even for trace
        // trc("{}", e.toString());
    });
}

void Window::init()
{
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

    setCallbacks();
}

void Window::destroy()
{
    glfwDestroyWindow(m_window);
}

void Window::setEventCallback(const EventCallback& callback)
{
    m_eventCallback = callback;
}

bool Window::shouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

void Window::setTitle(const std::string& title)
{
    m_properties.title = title;
    glfwSetWindowTitle(m_window, m_properties.title.c_str());
}

void Window::clearBuffers() const
{
    glm::vec4 color = m_properties.backgroundColor;
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

glm::dvec2 Window::getCursorPos() const
{
    glm::dvec2 position{};
    glfwGetCursorPos(m_window, &position.x, &position.y);
    return position;
}

void Window::setCursorPos(const glm::dvec2 position) const
{
    glfwSetCursorPos(m_window, position.x, position.y);
}

void Window::setMouseEnabled(const bool enabled) const
{
    if (enabled)
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

} // namespace core

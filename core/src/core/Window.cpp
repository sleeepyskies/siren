#include "Window.hpp"

namespace core
{

Window::Window(const Specification& specification = Specification())
    : m_specification(specification)
{
}

void Window::init()
{
    // specify version and features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_specification.width,
                                m_specification.height,
                                m_specification.title.c_str(),
                                nullptr,
                                nullptr);
    if (!m_window) {
        err("Could not create window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSwapInterval(m_specification.vSyncEnabled);

    registerResizeCallback([](GLFWwindow* win, int w, int h) { glViewport(0, 0, w, h); });
}

void Window::destroy()
{
    glfwDestroyWindow(m_window);
}

// TODO: replace this with emitting a Resize event!
void Window::registerResizeCallback(const std::function<void(GLFWwindow*, int, int)>& callback)
{
    m_callbacks.push_back(callback);
    glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);
}

void Window::frameBufferSizeCallback(GLFWwindow* window, const int width, const int height)
{
    const Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    for (const auto& cb : self->m_callbacks) {
        cb(window, width, height);
    }
}

bool Window::shouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

void Window::setTitle(const std::string& title)
{
    m_specification.title = title;
    glfwSetWindowTitle(m_window, m_specification.title.c_str());
}

void Window::clearBuffers() const
{
    glm::vec4 color = m_specification.backgroundColor;
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

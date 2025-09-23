#include "Window.hpp"

GLFWwindow* Window::m_window = nullptr;

Window::Window(const int width, const int height, const std::string& title)
{
    glfwInit();

    // specify version and features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        err("Could not create window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    setVsyncEnabled(false);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
}

void Window::registerCallback(const std::function<void(GLFWwindow*, int, int)>& callback)
{
    m_callbacks.push_back(callback);

    glfwSetFramebufferSizeCallback(m_window, Window::frameBufferSizeCallback);
}

void Window::frameBufferSizeCallback(GLFWwindow* window, const int width, const int height)
{
    const Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    for (const auto& cb : self->m_callbacks) {
        cb(window, width, height);
    }
}

bool Window::valid() const
{
    return m_window != nullptr;
}

bool Window::shouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

void Window::setShouldClose(const bool shouldClose) const
{
    glfwSetWindowShouldClose(m_window, shouldClose);
}

void Window::setBackgroundColor(const glm::vec4 color)
{
    m_backgroundColor = color;
}

void Window::setTitle(const std::string& title)
{
    m_title = title;
    glfwSetWindowTitle(m_window, m_title.c_str());
}

void Window::setVsyncEnabled(const bool enabled)
{
    m_vSyncEnabled = enabled;
    glfwSwapInterval(enabled);
}

void Window::clearBuffers() const
{
    clearColorBuffer();
    clearDepthBuffer();
}

void Window::clearColorBuffer() const
{
    glClearColor(
        m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::clearDepthBuffer() const
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

GLFWwindow* Window::handle()
{
    return m_window;
}

glm::ivec2 Window::getSize() const
{
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return { w, h };
}

int Window::getWidth() const
{
    return getSize().x;
}

int Window::getHeight() const
{
    return getSize().y;
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

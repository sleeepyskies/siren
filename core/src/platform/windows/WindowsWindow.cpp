#include "WindowsWindow.hpp"

#include "core/Debug.hpp"


namespace siren::platform
{

WindowsWindow::WindowsWindow(const Properties& properties) : Window(properties)
{
    // Make OpenGL Context class for this
    glfwSetErrorCallback(core::GLFWErrorCallback);
    glfwInit();

    // specify version and features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glfwSetWindowUserPointer(m_window, this);
    glfwSwapInterval(m_properties.vSyncEnabled);

    glfwSetWindowUserPointer(static_cast<GLFWwindow*>(handle()), this);

    // load opengl functions
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
    glDebugMessageCallback(core::OpenGLErrorCallback, nullptr);

    setupCallbacks();
}

WindowsWindow::~WindowsWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WindowsWindow::pollEvents()
{
    glfwPollEvents();
}

bool WindowsWindow::shouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

void WindowsWindow::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

glm::ivec2 WindowsWindow::getSize() const
{
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return { w, h };
}

void WindowsWindow::setTitle(const std::string& title)
{
    m_properties.title = title;
    glfwSetWindowTitle(m_window, m_properties.title.c_str());
}

void WindowsWindow::setVsync(const bool value)
{
    m_properties.vSyncEnabled = value;
    glfwSwapInterval(m_properties.vSyncEnabled);
    dbg("Vsync set to {}", value);
}

void* WindowsWindow::handle()
{
    return m_window;
}

void WindowsWindow::setScrollCallback(std::function<void(glm::vec2)> callback)
{
    m_scrollCallback = callback;
}

void WindowsWindow::setupCallbacks()
{
    glfwSetWindowUserPointer(m_window, this);

    glfwSetScrollCallback(m_window, glfwScrollCallback);
}

void WindowsWindow::glfwScrollCallback(GLFWwindow* win, double x, double y)
{
    const WindowsWindow* self = static_cast<WindowsWindow*>(glfwGetWindowUserPointer(win));
    if (self && self->m_scrollCallback) {
        self->m_scrollCallback({ x, y });
    }
}

} // namespace siren::platform

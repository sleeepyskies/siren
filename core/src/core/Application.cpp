#include "Application.hpp"

#include "core/GL.hpp"

#include "core/Debug.hpp"

namespace core
{

Application* Application::s_application = nullptr;

Application::Application(const Specification& specification = Specification())
{
    m_specification = specification;
    s_application   = this;
    // TODO: determine working directory

    glfwSetErrorCallback(debug::GLFWErrorCallback);
    glfwInit();

    if (m_specification.windowSpec.title.empty()) m_specification.windowSpec.title = "siren";
    m_window = makeRef<Window>(specification.windowSpec);
    m_window->init();

    // init any other singleton systems
    // ...

    // load opengl functions
    const int glVersion = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    if (!glVersion) {
        err("OpenGl could not load correctly");
        glfwTerminate();
    }

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    nfo("Loaded OpenGl version {}.{}", major, minor);

    // init OpenGL debug logging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug::OpenGLErrorCallback, nullptr);
}

Application::~Application()
{
    for (const auto& layer : m_layerStack) {
        layer->onDetach();
    }
    m_window->destroy();
    glfwTerminate();
    s_application = nullptr;
}

Application& Application::get()
{
    assert(s_application);
    return *s_application;
}

Window& Application::getWindow() const
{
    assert(s_application);
    assert(m_window);
    return *m_window;
}

void Application::run()
{
    m_running = true;

    while (m_running) {
        glfwPollEvents();
        m_window->clearBuffers();

        if (m_window->shouldClose()) {
            stop();
            break;
        }

        const float delta = m_time.delta();

        for (const auto& layer : m_layerStack) {
            layer->onUpdate(delta);
        }

        for (const auto& layer : m_layerStack) {
            layer->onRender();
        }

        m_window->swapBuffers();
    }
}

void Application::stop()
{
    m_running = false;
}

} // namespace core

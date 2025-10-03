#include "Application.hpp"

#include "core/GL.hpp"

#include "core/Debug.hpp"
#include "renderer/Renderer.hpp"

namespace core
{

Application* Application::s_instance = nullptr;

Application::Application(const Properties& specification)
{
    SirenAssert(!s_instance, "Cannot create multiple instances of Application");
    m_properties = specification;
    s_instance   = this;

    // determine working directory
    m_properties.workingDirectory = std::filesystem::current_path().parent_path();

    if (m_properties.windowProperties.title.empty()) m_properties.windowProperties.title = "siren";
    m_window = makeRef<Window>(specification.windowProperties);
    m_window->init();

    // init any other singleton systems
    m_assetManager = makeUref<assets::AssetManager>(m_properties.workingDirectory);

    // setup event callback system
    m_window->setEventCallback([this](Event& e) { this->onEvent(e); });

    renderer::Renderer::init();
}

void Application::onEvent(Event& e)
{
    m_eventQueue.push(e.createUref());
}

Application::~Application()
{
    for (const auto& layer : m_layerStack) {
        layer->onDetach();
    }
    m_window->destroy();
    glfwTerminate();
    s_instance = nullptr;
}

Application& Application::get()
{
    SirenAssert(s_instance, "Attempting to access Application before an instance has been made");
    return *s_instance;
}

Window& Application::getWindow() const
{
    SirenAssert(s_instance,
                "Attempting to access Window before an instance of Application has been made");
    return *m_window;
}

assets::AssetManager& Application::getAssetManager() const
{
    return *m_assetManager;
}

const Application::Properties& Application::getProperties()
{
    return m_properties;
}

void Application::run()
{
    m_running = true;

    while (m_running) {
        glfwPollEvents();

        while (!m_eventQueue.empty()) {
            Uref<Event> e = std::move(m_eventQueue.front());
            m_eventQueue.pop();
            for (const auto& layer : m_layerStack) {
                layer->onEvent(*e);
                if (e->isHandled()) break;
            }
        }

        if (m_window->shouldClose()) {
            stop();
            break;
        }

        const float delta = m_time.delta();

        for (const auto& layer : m_layerStack) {
            layer->onUpdate(delta);
        }

        m_window->clearBuffers();

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

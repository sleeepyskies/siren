#include "Application.hpp"

#include "Input.hpp"
#include "core/GL.hpp"

#include "renderer/Renderer.hpp"

namespace siren::core
{

Application* Application::s_instance = nullptr;

Application::Application(const Properties& properties)
{
    SirenAssert(!s_instance, "Cannot create multiple instances of Application");
    m_properties = properties;
    s_instance   = this;

    if (m_properties.windowProperties.title.empty()) {
        m_properties.windowProperties.title = "siren";
    }
    m_window = makeRef<Window>(properties.windowProperties);
    m_window->init();

    // init any other singleton systems
    m_assetManager      = makeUref<assets::AssetManager>();
    m_shaderManager     = makeUref<renderer::ShaderManager>();
    m_fileSystemManager = makeUref<FileSystemManager>();

    // setup event callback system
    m_window->setEventCallback([this](const event::Event& e) { this->onEvent(e); });

    renderer::Renderer::init();
}

void Application::onEvent(const event::Event& e)
{
    m_eventQueue.push(e.createUref());
}

Application::~Application()
{
    for (const auto& layer : m_layerStack) { layer->onDetach(); }
    m_window->destroy();
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

const Time& Application::getTime() const
{
    SirenAssert(s_instance,
                "Attempting to access Time before an instance of Application has been made");
    return m_time;
}

assets::AssetManager& Application::getAssetManager() const
{
    return *m_assetManager;
}

renderer::ShaderManager& Application::getShaderManager() const
{
    return *m_shaderManager;
}

assets::AssetRegistry& Application::getAssetRegistry() const
{
    return m_assetManager->getAssetRegistry();
}

FileSystemManager& Application::getFileSystemManager() const
{
    return *m_fileSystemManager;
}

const Application::Properties& Application::getProperties()
{
    return m_properties;
}

void Application::run()
{
    m_running = true;

    while (m_running) {
        Input::update();
        glfwPollEvents();

        while (!m_eventQueue.empty()) {
            Uref<event::Event> e = std::move(m_eventQueue.front());
            m_eventQueue.pop();
            for (const auto& layer : m_layerStack) {
                layer->onEvent(*e);
                if (e->isHandled()) { break; }
            }
        }

        if (m_window->shouldClose()) {
            stop();
            break;
        }

        const float delta = m_time.delta();

        for (const auto& layer : m_layerStack) { layer->onUpdate(delta); }

        m_window->clearBuffers();

        for (const auto& layer : m_layerStack) { layer->onRender(); }

        for (const auto& layer : m_layerStack) { layer->onUiRender(); }

        m_window->swapBuffers();
    }
}

void Application::stop()
{
    m_running = false;
}

} // namespace siren::core

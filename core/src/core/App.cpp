#include "App.hpp"

#include "Module.hpp"
#include "input/InputModule.hpp"
#include "time/TimeModule.hpp"
#include "window/WindowModule.hpp"
#include "assets/AssetModule.hpp"
#include "renderer/RenderModule.hpp"
#include "filesystem/FileSystemModule.hpp"

#include <ranges>


namespace siren::core
{

App& App::create(const Properties& properties)
{
    SirenAssert(!s_instance, "Cannot create multiple instances of Application");
    s_instance = new App(properties);
    SirenAssert(s_instance, "App initialization failed");

    // all required modules should be added here. order is important
    s_instance->init();
    return *s_instance;
}

App& App::get()
{
    SirenAssert(s_instance, "Attempting to access Application before an instance has been made");
    return *s_instance;
}

void App::init()
{
    s_instance->registerModule<FileSystemModule>();
    s_instance->registerModule<WindowModule>();
    s_instance->registerModule<InputModule>();
    s_instance->registerModule<TimeModule>();
    s_instance->registerModule<AssetModule>();
    s_instance->registerModule<RenderModule>();
}


void App::popLayer(Layer* layer)
{
    const auto it =
            std::ranges::find_if(m_layers, [layer] (const Own<Layer>& l) { return l.get() == layer; });

    if (it != m_layers.end()) {
        (*it)->onDetach();
        m_layers.erase(it);
    }
}

void App::run()
{
    bool running = true;

    // cache access to core modules
    auto* input    = getModule<InputModule>();
    auto* window   = getModule<WindowModule>();
    auto* time     = getModule<TimeModule>();
    auto* renderer = getModule<RenderModule>();

    while (running) {
        const float delta = time->delta();

        input->update();
        window->pollEvents();

        running = !window->shouldClose();

        if (!running) {
            break;
        }

        for (const auto& layer : m_layers) {
            layer->onUpdate(delta);
        }

        for (const auto& layer : m_layers) {
            layer->onRender();
        }

        for (const auto& layer : m_layers) {
            layer->onUiRender();
        }

        window->swapBuffers();
    }

    // fixme: in what order?
    // shutdown all modules
    for (const auto& module : m_modules | std::views::values) {
        module->shutdown();
    }
}

void App::switchRenderAPI(const Properties::RenderAPI renderAPI)
{
    if (renderAPI == m_properties.renderAPI) {
        return;
    } // no work to be done :D

    m_properties.renderAPI = renderAPI;
    // todo: reinit things like window, renderer, time
}

App::Properties App::getProperties() const
{
    return m_properties;
}

App::App(const Properties& properties) : m_properties(properties)
{
    s_instance = this;
}

App::~App()
{
    for (const auto& val : m_modules | std::views::values) {
        val->shutdown();
    }
    s_instance = nullptr;
}

} // namespace siren::core

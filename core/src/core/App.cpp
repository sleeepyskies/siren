#include "App.hpp"

#include "Timer.hpp"
#include "Module.hpp"
#include "input/InputModule.hpp"
#include "window/WindowModule.hpp"

#include <ranges>

#include "events/Events.hpp"


namespace siren::core
{
App& App::get()
{
    SirenAssert(s_instance, "Attempting to access Application before an instance has been made");
    return *s_instance;
}

void App::run()
{
    Timer::initialize();

    // cache access to core modules
    auto* input  = getModule<InputModule>();
    auto* window = getModule<WindowModule>();

    while (m_running) {
        Timer::tick();
        input->update();
        window->pollEvents();
        m_eventBus.dispatch();

        if (!m_running) { break; } // handled via emit event

        s_instance->onUpdate(Timer::getDelta());
        s_instance->onRender();

        window->swapBuffers();
    }

    // fixme: in what order to shutdown?
    for (const auto& module : m_modules | std::views::values) {
        module->Shutdown();
    }
}

void App::initialize()
{
    // these are seen as core and siren cannot work without
    s_instance->registerModule<WindowModule>();
    s_instance->registerModule<InputModule>();

    m_eventBus.subscribe<AppCloseEvent>(
        [this] (auto& event) {
            m_running = false;
            return false;
        }
    );
}

void App::switchRenderAPI(const Properties::RenderAPI renderAPI)
{
    // no work to be done :D
    if (renderAPI == m_properties.renderAPI) { return; }

    m_properties.renderAPI = renderAPI;
    // todo: reinit things like window, renderer, time
}

App::Properties App::getProperties() const
{
    return m_properties;
}

EventBus& App::getEventBus()
{
    return m_eventBus;
}

App::App(const Properties& properties) : m_properties(properties)
{
    s_instance = this;
    s_instance->initialize();
}

App::~App()
{
    for (const auto& val : m_modules | std::views::values) {
        val->Shutdown();
    }
    s_instance = nullptr;
}
} // namespace siren::core

#include "App.hpp"

#include "Timer.hpp"
#include "Module.hpp"
#include "input/InputModule.hpp"
#include "window/WindowModule.hpp"
#include "events/Events.hpp"

#include <ranges>


namespace siren::core
{
App& App::Get()
{
    SirenAssert(s_instance, "Attempting to access Application before an instance has been made");
    return *s_instance;
}

void App::Run()
{
    Timer::initialize();

    // cache access to core modules
    auto* input  = GetModule<InputModule>();
    auto* window = GetModule<WindowModule>();

    while (m_running) {
        Timer::tick();
        input->update();
        window->pollEvents();
        m_eventBus.Dispatch();

        if (!m_running) { break; } // handled via emit event

        s_instance->OnUpdate(Timer::getDelta());
        s_instance->OnRender();

        window->swapBuffers();
    }

    // fixme: in what order to shutdown?
    for (const auto& module : m_modules | std::views::values) {
        module->Shutdown();
    }
}

void App::Init()
{
    // these are seen as core and siren cannot work without
    s_instance->RegisterModule<WindowModule>();
    s_instance->RegisterModule<InputModule>();

    m_eventBus.Subscribe<AppCloseEvent>(
        [this] (auto& event) {
            m_running = false;
            return false;
        }
    );
}

void App::SwitchRenderApi(const Properties::RenderAPI renderAPI)
{
    // no work to be done :D
    if (renderAPI == m_properties.renderAPI) { return; }

    m_properties.renderAPI = renderAPI;
    // todo: reinit things like window, renderer, time
}

App::Properties App::GetProperties() const
{
    return m_properties;
}

EventBus& App::GetEventBus()
{
    return m_eventBus;
}

App::App(const Properties& properties) : m_properties(properties)
{
    s_instance = this;
    s_instance->Init();
}

App::~App()
{
    for (const auto& val : m_modules | std::views::values) {
        val->Shutdown();
    }
    s_instance = nullptr;
}
} // namespace siren::core

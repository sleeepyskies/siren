#include "App.hpp"

#include <ranges>

#include "Timer.hpp"
#include "input/InputModule.hpp"
#include "window/WindowModule.hpp"
#include "events/Events.hpp"
#include "events/EventBus.hpp"
#include "Locator.hpp"
#include "TaskPool.hpp"
#include "assets/AssetServer.hpp"
#include "renderer/RenderModule.hpp"


namespace siren::core
{
App& App::get() {
    SIREN_ASSERT(s_instance, "Attempting to access Application before an instance has been made");
    return *s_instance;
}

void App::run() const {
    Timer::init();

    // cache access to core modules
    auto& input        = Locator<InputModule>::locate();
    const auto& window = Locator<WindowModule>::locate();
    auto& event_bus    = Locator<EventBus>::locate();
    auto& asset_server = Locator<AssetServer>::locate();

    while (m_running) {
        Timer::tick();
        input.update();
        window.poll_events();
        event_bus.dispatch();

        if (!m_running) { break; } // handled via emit event

        s_instance->on_update(Timer::get_delta());
        s_instance->on_render();

        window.swap_buffers();
    }
}

void App::init() {
    // init core systems
    Locator<EventBus>::provide(new EventBus());
    Locator<TaskPool>::provide(new TaskPool());
    Locator<WindowModule>::provide(new WindowModule());
    Locator<InputModule>::provide(new InputModule());
    Locator<AssetServer>::provide(new AssetServer());
    Locator<Renderer>::provide(new Renderer());
    Locator<App>::provide(this);

    Locator<EventBus>::locate().subscribe<AppCloseEvent>(
        [this] (auto&) {
            m_running = false;
            return false;
        }
    );
}

void App::switch_render_api(const Properties::RenderAPI api) {
    // no work to be done :D
    if (api == m_properties.renderAPI) { return; }

    m_properties.renderAPI = api;
    // todo: reinit things like window, renderer, time
}

App::Properties App::properties() const {
    return m_properties;
}

App::App(const Properties& properties) : m_properties(properties) {
    s_instance = this;
    s_instance->init();
}

App::~App() {
    // todo: handle shutdown here
    s_instance = nullptr;
}
} // namespace siren::core

#include "app.hpp"

#include "assets/asset_server.hpp"
#include "event_bus.hpp"
#include "core/events.hpp"
#include "input/input.hpp"
#include "locator.hpp"
#include "logger.hpp"
#include "renderer/renderer.hpp"
#include "sync/thread_pool.hpp"
#include "time.hpp"

#include "renderer/device.hpp"

#include "window/window.hpp"


namespace siren::core
{

auto App::run() -> void {
    Time::init();

    log()->info("Running main loop!");

    while (m_running) {
        Time::tick();
        Locator<Window>::value().poll_events();
        Locator<Input>::value().update();
        Locator<EventBus>::value().dispatch();

        if (!m_running) {
            break;
        }

        if (!Locator<Window>::value().is_minimized()) {
            this->on_update(Time::delta());
            this->on_render();
        }

        Locator<Renderer>::value().device()->wait_until_idle();
        Locator<Renderer>::value().device()->present();
    }

    log()->info("Main loop ended!");
}

App::App(const Config& config) : m_running(true), m_config(config) { }

App::~App() {
    log()->info("Shutting down core systems...");

    // reverse order

    Locator<Renderer>::reset();
    Locator<Window>::reset();
    Locator<Input>::reset();
    Locator<AssetServer>::reset();
    Locator<ThreadPool>::reset();
    Locator<EventBus>::reset();

    log()->info("Core systems shutdown");
}

void App::init() {
    Locator<Logger>::emplace(m_config.logging_config);

    log()->info("Initialising core systems...");

    Locator<EventBus>::emplace();                          // no deps
    Locator<ThreadPool>::emplace(-1);                      // no deps, keep one for render thread
    Locator<AssetServer>::emplace(m_config.assets_config); // no deps
    Locator<Input>::emplace();                             // no deps
    Locator<Window>::emplace(m_config.window_config);      // requires input and event bus to be setup
    Locator<Renderer>::emplace(m_config.renderer_config);  // todo: not sure what it needs yet

    Locator<EventBus>::value().subscribe<WindowCloseEvent>(
        [this] (auto&) {
            m_running = false;
            return false;
        }
    );

    log()->info("Core initialized.");

    this->on_init();
}
} // namespace siren::core

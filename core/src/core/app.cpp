#include "app.hpp"

#include "assets/asset_server.hpp"
#include "event_bus.hpp"
#include "core/events.hpp"
#include "input/input_module.hpp"
#include "locator.hpp"
#include "logger.hpp"
#include "renderer/renderer.hpp"
#include "sync/render_thread.hpp"
#include "sync/thread_pool.hpp"
#include "time.hpp"

#include "platform/opengl/opengl_device.hpp"

#include "window/window.hpp"


namespace siren::core
{

auto App::run() -> void {
    Time::init();

    while (m_running) {
        Time::tick();
        Locator<Input>::value().update();
        Locator<Window>::value().poll_events();
        Locator<EventBus>::value().dispatch();

        // stop handled via events
        if (!m_running) {
            break;
        }

        this->on_update(Time::delta());
        this->on_render();

        window.swap_buffers();
    }
}

App::App(const Config& config) : m_running(true), m_config(config) { }

App::~App() {
    // todo: handle shutdown here
    Locator<EventBus>::reset();
    Locator<ThreadPool>::reset();
    Locator<RenderThread>::reset();
    Locator<WindowModule>::reset();
    Locator<InputModule>::reset();
    Locator<Device>::reset();
    Locator<AssetServer>::reset();
    Locator<Renderer>::reset();
}

void App::init() {
    Locator<Logger>::emplace(m_config.logging_config);
    m_log = Locator<Logger>::value().core;

    m_log->info("Initialising core systems...");

    Locator<EventBus>::emplace();
    Locator<ThreadPool>::emplace();
    Locator<RenderThread>::emplace();
    Locator<WindowModule>::emplace();
    Locator<InputModule>::emplace();
    Locator<Device>::emplace();
    Locator<AssetServer>::emplace();
    Locator<Renderer>::emplace();
    Locator<App>::emplace(this);

    Locator<EventBus>::value().subscribe<AppCloseEvent>(
        [this] (auto&) {
            m_running = false;
            return false;
        }
    );

    this->on_init();
}
} // namespace siren::core

#include "App.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"

#include "Time.hpp"
#include "input/InputModule.hpp"
#include "window/WindowModule.hpp"
#include "events/Events.hpp"
#include "events/EventBus.hpp"
#include "Locator.hpp"
#include "Logger.hpp"
#include "../sync/task_pool.hpp"
#include "assets/AssetServer.hpp"
#include "renderer/Renderer.hpp"


namespace siren::core
{
App& App::get() {
    SIREN_ASSERT(s_instance, "Attempting to access Application before an instance has been made");
    return *s_instance;
}

void App::run() const {
    Time::init();

    // cache access to core modules
    auto& input        = Locator<InputModule>::locate();
    const auto& window = Locator<WindowModule>::locate();
    auto& event_bus    = Locator<EventBus>::locate();

    while (m_running) {
        Time::tick();
        input.update();
        window.poll_events();
        event_bus.dispatch();

        if (!m_running) { break; } // handled via emit event

        s_instance->on_update(Time::delta());
        s_instance->on_render();

        window.swap_buffers();
    }
}

void App::init() {
    // init core systems
    Locator<EventBus>::provide(new EventBus());
    Locator<TaskPool>::provide(new TaskPool());
    Locator<RenderThread>::provide(new RenderThread());
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

void App::switch_render_api(const Description::RenderAPI api) {
    // no work to be done :D
    if (api == m_description.renderAPI) { return; }

    m_description.renderAPI = api;
    // todo: reinit things like window, renderer, time
}

App::Description App::description() const { return m_description; }

App::App(const Description& properties) : m_description(properties) {
    s_instance = this;
    s_instance->init();

    // setup logging
    {
        spdlog::init_thread_pool(1024, 1);
        const auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console->set_pattern("[%Y-%m-%d %H:%M:%S] [thread %t] [%n] [%^%l%$] [%s:%#] %v");

        const std::vector<std::string> systems = { "Core", "Assets", "ECS", "Renderer", "UI" };

        for (const auto& system_name : systems) {
            auto logger = std::make_shared<spdlog::async_logger>(
                system_name,
                console,
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::block
            );
            spdlog::register_logger(logger);
        }

        Logger::core     = spdlog::get("core");
        Logger::assets   = spdlog::get("Assets");
        Logger::ecs      = spdlog::get("ECS");
        Logger::renderer = spdlog::get("Renderer");
        Logger::ui       = spdlog::get("UI");

        spdlog::flush_on(spdlog::level::warn);
        spdlog::set_default_logger(spdlog::get("Core"));
    }
}

App::~App() {
    // todo: handle shutdown here
    s_instance = nullptr;
    Locator<EventBus>::terminate();
    Locator<TaskPool>::terminate();
    Locator<RenderThread>::terminate();
    Locator<WindowModule>::terminate();
    Locator<InputModule>::terminate();
    Locator<AssetServer>::terminate();
    Locator<Renderer>::terminate();
}
} // namespace siren::core

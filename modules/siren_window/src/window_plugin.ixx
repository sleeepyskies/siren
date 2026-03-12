export module siren.window:window_plugin;

import siren.app.plugin;
import siren.app.app;
import :window;
import :config;
import :signals;
import :systems;

namespace siren::window {

auto window_main_loop(App& app) {
    log::info("Starting window main loop.");

    auto& window = app.world().resource<Window>();

    while (window) {
        window.poll_events();
        app.step();
    }

    log::info("Main window loop ended.");
}

export class WindowPlugin final : public Plugin {
public:
    explicit WindowPlugin(const WindowConfig& config) : m_config(config) { }

    auto construct(App& app) const -> void override;
    auto shutdown(App& app) const -> void override;

private:
    WindowConfig m_config;
};

auto WindowPlugin::construct(App& app) const -> void {
    auto& signals = app.world().resource<signal::SignalBus>();

    app.set_loop(window_main_loop);
    app.world().add_resource<Window>(m_config);
}

auto WindowPlugin::shutdown(App& app) const -> void {
    app.world().remove_resource<Window>();
}
} // namespace siren::window

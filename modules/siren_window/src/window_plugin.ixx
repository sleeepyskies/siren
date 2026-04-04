export module siren.window:window_plugin;

import :window;
import :config;
import :events;

import siren.app.app;
import siren.app.plugin;
import siren.ecs;
import siren.schedule;

namespace siren::window {

/**
 * @brief Resource storing data needed for handling glfw callbacks.
 */
export struct WindowEventState {
    /** @brief Cached window. */
    Window* window;
    /** @brief Cached event bus. */
    ecs::EventBus* event_bus;
    /** @brief Cached signal bus. */
    ecs::SignalBus* signal_bus;
};

/**
 * @class WindowPlugin
 * @brief Sets up window related systems and state.
 */
export class WindowPlugin final : public Plugin {
public:
    explicit WindowPlugin(const WindowConfig& config) : m_config(config) { }

    /** @brief Sets up the WindowPlugin. */
    auto construct(App& app) const -> void override;
    /** @brief Handles cleanup for the WindowPlugin. */
    auto shutdown(App& app) const -> void override;

    /** @brief Handles setting up glfw callbacks for a specific window. */
    auto register_glfw_callbacks(ecs::Resource<WindowEventState&> wes) const -> void;

private:
    WindowConfig m_config;
};
} // namespace siren::window

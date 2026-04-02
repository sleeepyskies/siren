export module input_plugin;

import siren.window;

import siren.app.plugin;
import siren.app.app;
import siren.ecs.system;

import siren.input.input;
import siren.input.input_codes;
import siren.input.mappings;

namespace siren::input {

/// @todo: we want some way to say add_system(PreUpdate, (update_input).before(window_poll_events))

/**
 * @brief Registers the input modules signal callbacks.
 */
auto register_signal_callbacks(
    ecs::SignalBus& signals
) -> void {
    signals.on<window::GLFWKeyPressedSignal>().run(
        [] (const window::GLFWKeyPressedSignal& event, ecs::Resource<KeyInput&> keys) {
            InputProxy::press(*keys, from_glfw_key(event.code));
        }
    );

    signals.on<window::GLFWKeyReleasedSignal>().run(
        [] (const window::GLFWKeyReleasedSignal& event, ecs::Resource<KeyInput&> keys) {
            InputProxy::release(*keys, from_glfw_key(event.code));
        }
    );

    signals.on<window::GLFWMouseButtonPressedSignal>.run(
        [] (const window::GLFWMouseButtonPressedSignal& event, ecs::Resource<MouseInput&> keys) {
            InputProxy::press(*keys, from_glfw_mouse(event.button));
        }
    );

    signals.on<window::GLFWMouseButtonReleasedSignal>.run(
        [] (const window::GLFWMouseButtonReleasedSignal& event, ecs::Resource<MouseInput&> keys) {
            InputProxy::release(*keys, from_glfw_mouse(event.button));
        }
    );

    signals.on<window::GLFWMouseScrollSignal>.run(
        [] (const window::GLFWMouseScrollSignal& event, ecs::Resource<MouseMovement&> movement) {
            InputProxy::on_mouse_scroll(*movement, event.offset);
        }
    );

    signals.on<window::GLFWMouseMotionSignal>.run(
        [] (const window::GLFWMouseMotionSignal& event, ecs::Resource<MouseMovement&> movement) {
            InputProxy::on_mouse_move(*movement, event.pos);
        }
    );
}

export class InputPlugin final : public Plugin {
    auto construct(App& app) const -> void override;
    auto shutdown(App& app) const -> void override;
};

auto InputPlugin::construct(App& app) const -> void {
    app
           .add_resources<MouseInput, KeyInput, MouseMovement>()
           .add_system(schedule::SchedulePhase::First, register_signal_callbacks)
           .add_system(schedule::SchedulePhase::PreUpdate, (InputProxy::update).after(window::poll_window_events));
}

auto InputPlugin::shutdown(App& app) const -> void {
    app.remove_resources<ButtonState<Mouse>, ButtonState<Key>, MouseMovement>();
}

} // namespace siren::input

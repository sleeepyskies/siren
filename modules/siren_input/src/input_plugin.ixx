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

auto register_signal_callbacks(
    ecs::SignalBus& signals
) -> void {
    signals.on<window::GLFWKeyPressedSignal>(
        [] (const window::GLFWKeyPressedSignal& event, ecs::Resource<KeyInput&> keys) {
            keys->press(from_glfw_key(event.code));
        }
    );

    signals
           .on<window::GLFWKeyPressedSignal>()
           .target(some_entity)
           .run(
                [] (const window::GLFWKeyPressedSignal& event, ecs::Resource<KeyInput&> keys) {
                    keys->press(from_glfw_key(event.code));
                }
            );

    signals.on(
        [] (const window::GLFWKeyPressedSignal& event, ecs::Resource<KeyInput&> keys) {
            keys->press(from_glfw_key(event.code));
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
           .add_system(schedule::SchedulePhase::PreUpdate, );
}

auto InputPlugin::shutdown(App& app) const -> void {
    app.remove_resources<ButtonState<Mouse>, ButtonState<Key>, MouseMovement>();
}

} // namespace siren::input

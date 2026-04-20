export module siren.window:systems;

import siren.ecs;
import :window;

namespace siren::window {

/**
 * @brief Handles polling window events from glfw.
 */
export auto poll_window_events(ecs::Resource<Window> window) -> void {
    window->poll_events();
}

} // namespace siren::window

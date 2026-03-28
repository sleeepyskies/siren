export module siren.window:events;

import siren.math;

export namespace siren::window {

struct WindowResizeEvent {
    glm::uvec2 size;
};

struct WindowMoveEvent {
    glm::ivec2 position;
};

struct WindowMinimizedEvent { };

struct WindowMaximizedEvent { };

struct WindowClosedEvent { };

} // namespace siren::window

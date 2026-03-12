export module siren.window:signals;

import siren.math;

export namespace siren::window {

struct WindowResizeSignal {
    glm::uvec2 size;
};

struct WindowMoveSignal {
    glm::ivec2 position;
};

struct WindowMinimizedSignal { };

struct WindowMaximizedSignal { };

struct WindowClosedSignal { };

} // namespace siren::window

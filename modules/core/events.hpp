#pragma once

#include "input/input_codes.hpp"
#include "spch.hpp"


namespace siren::core {
struct WindowResizeEvent {
    glm::uvec2 size;
};

struct WindowMoveEvent {
    glm::ivec2 position;
};

struct WindowMinimizedEvent { };

struct WindowMaximizedEvent { };

struct ScrollEvent {
    glm::vec2 delta;
};

struct KeyboardButtonPressedEvent {
    Key key;
    Modifiers mods;
};

struct KeyboardButtonReleasedEvent {
    Key key;
};

struct MouseButtonPressedEvent {
    Mouse key;
    Modifiers mods;
};

struct MouseButtonReleasedEvent {
    Mouse key;
};

struct MouseMotionEvent {
    glm::vec2 position;
};

struct WindowCloseEvent { };
} // namespace siren::core

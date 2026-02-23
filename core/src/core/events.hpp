#pragma once

#include "input/input_codes.hpp"
#include "spch.hpp"


namespace siren::core
{
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

struct KeyPressedEvent {
    KeyCode key;
    Modifiers mods;
};

struct KeyReleasedEvent {
    KeyCode key;
};

struct MouseKeyPressedEvent {
    MouseCode key;
    Modifiers mods;
};

struct MouseKeyReleasedEvent {
    MouseCode key;
};

struct MouseMovedEvent {
    glm::vec2 position;
};

struct WindowCloseEvent { };
} // namespace siren::core

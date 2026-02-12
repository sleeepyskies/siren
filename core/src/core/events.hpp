#pragma once

#include "input/InputCodes.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
struct WindowResizeEvent {
    i32 width, height;
};

struct ScrollEvent {
    double x, y;
};

struct KeyPressedEvent {
    KeyCode key;
};

struct KeyReleasedEvent {
    KeyCode key;
};

struct MouseKeyPressedEvent {
    MouseCode key;
};

struct MouseKeyReleasedEvent {
    MouseCode key;
};

struct MouseMovedEvent {
    double x, y;
};

struct AppCloseEvent { };
}

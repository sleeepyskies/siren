export module siren.window:signals;

import siren.common;
import siren.math;

export namespace siren::window {

struct GLFWKeyPressedSignal {
    i32 code;
    i32 scancode;
    i32 mods;
};

struct GLFWKeyReleasedSignal {
    i32 code;
    i32 scancode;
    i32 mods;
};

struct GLFWMouseButtonPressedSignal {
    i32 button;
    i32 mods;
};

struct GLFWMouseButtonReleasedSignal {
    i32 button;
    i32 mods;
};

struct GLFWMouseMotionSignal {
    glm::vec2 pos;
};

struct GLFWMouseScrollSignal {
    glm::vec2 offset;
};

} // namespace siren::window

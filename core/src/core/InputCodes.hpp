#pragma once

#include "core/GL.hpp"

namespace siren::core
{
// TODO: toString functionality for debugging/logggin purposes
// TODO: Some mapping of actions to groups? Like "moveLeft" -> {A_KEY, X_BUTTON}

/// @brief Internal enum alias for GLFW keycodes
enum class KeyCode {
    // Letter keys

    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,

    // Modifier Keys
    ESC     = GLFW_KEY_ESCAPE,
    SPACE   = GLFW_KEY_SPACE,
    L_SHIFT = GLFW_KEY_LEFT_SHIFT,

    // Function Keys
    F1  = GLFW_KEY_F1,
    F2  = GLFW_KEY_F2,
    F3  = GLFW_KEY_F3,
    F4  = GLFW_KEY_F4,
    F5  = GLFW_KEY_F5,
    F6  = GLFW_KEY_F6,
    F7  = GLFW_KEY_F7,
    F8  = GLFW_KEY_F8,
    F9  = GLFW_KEY_F9,
    F10 = GLFW_KEY_F10,
    F11 = GLFW_KEY_F11,
    F12 = GLFW_KEY_F12,

    // arrow keys
    ARROW_LEFT  = GLFW_KEY_LEFT,
    ARROW_RIGHT = GLFW_KEY_RIGHT,
    ARROW_UP    = GLFW_KEY_UP,
    ARROW_DOWN  = GLFW_KEY_DOWN,

    // Page Keys
    PAGE_UP   = GLFW_KEY_PAGE_UP,
    PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
};

enum class MouseCode {
    LEFT   = GLFW_MOUSE_BUTTON_LEFT,
    RIGHT  = GLFW_MOUSE_BUTTON_RIGHT,
    MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
};

enum class MouseMode {
    /// @brief Regular visible cursor
    VISIBLE,
    /// @brief Regular visible cursor
    INVISIBLE,
    /// @brief Cursor is invisible and cannot leave the window and is centered each frame
    LOCKED,
};

} // namespace siren::core

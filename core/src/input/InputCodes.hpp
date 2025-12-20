/**
 * @file InputCodes.hpp
 * @brief File containing input relevant enums.
 */
#pragma once

namespace siren::core
{
// todo: toString functionality for debugging/logging purposes
// todo: Some mapping of actions to groups? Like "moveLeft" -> {A_KEY, X_BUTTON}

/// @brief All keyboard buttons.
enum class KeyCode
{
    SPACE = 0,
    APOSTROPHE,
    COMMA,
    MINUS,
    PERIOD,
    SLASH,
    NUM_0,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    SEMICOLON,
    EQUAL,
    LEFT_BRACKET,
    BACKSLASH,
    RIGHT_BRACKET,
    GRAVE_ACCENT,
    ESC,
    ENTER,
    TAB,
    BACKSPACE,
    INSERT,
    DELETE_KEY,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    CAPS_LOCK,
    SCROLL_LOCK,
    NUM_LOCK,
    PRINT_SCREEN,
    PAUSE,
    KP_DECIMAL,
    KP_DIVIDE,
    KP_MULTIPLY,
    KP_SUBTRACT,
    KP_ADD,
    KP_ENTER,
    KP_EQUAL,
    L_SHIFT,
    L_CONTROL,
    L_ALT,
    L_SUPER,
    R_SHIFT,
    R_CONTROL,
    R_ALT,
    R_SUPER,
    MENU,
    KEY_LAST,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,

    MAX, // Do not use
};

/// @brief Mouse buttons.
enum class MouseCode
{
    LEFT = 0,
    RIGHT,
    MIDDLE,

    MAX, // Do not use
};

/// @brief Cursor behaviour types.
enum class MouseMode
{
    /// @brief Indicates some invalid state. May occur when querying too soon etc.
    INVALID = 0,
    /// @brief Regular visible cursor
    VISIBLE,
    /// @brief Regular visible cursor
    INVISIBLE,
    /// @brief Cursor is invisible and cannot leave the window and is centered each frame
    LOCKED,
};
} // namespace siren::core

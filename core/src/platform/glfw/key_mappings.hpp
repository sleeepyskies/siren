#pragma once

#include <GLFW/glfw3.h>
#include "input/input_codes.hpp"
#include "core/spch.hpp"


namespace siren::platform
{

/**
 * @brief Maps siren KeyCode's to native GLFW.
 * @param key The siren @ref KeyCode to map.
 * @return An unsigned integer representing a GLFW key.
 */
inline auto to_glfw(const core::Key key) -> u32 {
    switch (key) {
        case core::Key::SPACE: return GLFW_KEY_SPACE;
        case core::Key::APOSTROPHE: return GLFW_KEY_APOSTROPHE;
        case core::Key::COMMA: return GLFW_KEY_COMMA;
        case core::Key::MINUS: return GLFW_KEY_MINUS;
        case core::Key::PERIOD: return GLFW_KEY_PERIOD;
        case core::Key::SLASH: return GLFW_KEY_SLASH;
        case core::Key::NUM_0: return GLFW_KEY_0;
        case core::Key::NUM_1: return GLFW_KEY_1;
        case core::Key::NUM_2: return GLFW_KEY_2;
        case core::Key::NUM_3: return GLFW_KEY_3;
        case core::Key::NUM_4: return GLFW_KEY_4;
        case core::Key::NUM_5: return GLFW_KEY_5;
        case core::Key::NUM_6: return GLFW_KEY_6;
        case core::Key::NUM_7: return GLFW_KEY_7;
        case core::Key::NUM_8: return GLFW_KEY_8;
        case core::Key::NUM_9: return GLFW_KEY_9;
        case core::Key::SEMICOLON: return GLFW_KEY_SEMICOLON;
        case core::Key::EQUAL: return GLFW_KEY_EQUAL;
        case core::Key::LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
        case core::Key::BACKSLASH: return GLFW_KEY_BACKSLASH;
        case core::Key::RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;
        case core::Key::GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;
        case core::Key::ESC: return GLFW_KEY_ESCAPE;
        case core::Key::ENTER: return GLFW_KEY_ENTER;
        case core::Key::TAB: return GLFW_KEY_TAB;
        case core::Key::BACKSPACE: return GLFW_KEY_BACKSPACE;
        case core::Key::INSERT: return GLFW_KEY_INSERT;
        case core::Key::DELETE_KEY: return GLFW_KEY_DELETE;
        case core::Key::PAGE_UP: return GLFW_KEY_PAGE_UP;
        case core::Key::PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
        case core::Key::HOME: return GLFW_KEY_HOME;
        case core::Key::END: return GLFW_KEY_END;
        case core::Key::CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
        case core::Key::SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
        case core::Key::NUM_LOCK: return GLFW_KEY_NUM_LOCK;
        case core::Key::PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
        case core::Key::PAUSE: return GLFW_KEY_PAUSE;
        case core::Key::KP_DECIMAL: return GLFW_KEY_KP_DECIMAL;
        case core::Key::KP_DIVIDE: return GLFW_KEY_KP_DIVIDE;
        case core::Key::KP_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
        case core::Key::KP_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
        case core::Key::KP_ADD: return GLFW_KEY_KP_ADD;
        case core::Key::KP_ENTER: return GLFW_KEY_KP_ENTER;
        case core::Key::KP_EQUAL: return GLFW_KEY_KP_EQUAL;
        case core::Key::L_SHIFT: return GLFW_KEY_LEFT_SHIFT;
        case core::Key::L_CONTROL: return GLFW_KEY_LEFT_CONTROL;
        case core::Key::L_ALT: return GLFW_KEY_LEFT_ALT;
        case core::Key::L_SUPER: return GLFW_KEY_LEFT_SUPER;
        case core::Key::R_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
        case core::Key::R_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
        case core::Key::R_ALT: return GLFW_KEY_RIGHT_ALT;
        case core::Key::R_SUPER: return GLFW_KEY_RIGHT_SUPER;
        case core::Key::MENU: return GLFW_KEY_MENU;
        case core::Key::KEY_LAST: return GLFW_KEY_LAST;
        case core::Key::ARROW_LEFT: return GLFW_KEY_LEFT;
        case core::Key::ARROW_RIGHT: return GLFW_KEY_RIGHT;
        case core::Key::ARROW_UP: return GLFW_KEY_UP;
        case core::Key::ARROW_DOWN: return GLFW_KEY_DOWN;

        case core::Key::A: return GLFW_KEY_A;
        case core::Key::B: return GLFW_KEY_B;
        case core::Key::C: return GLFW_KEY_C;
        case core::Key::D: return GLFW_KEY_D;
        case core::Key::E: return GLFW_KEY_E;
        case core::Key::F: return GLFW_KEY_F;
        case core::Key::G: return GLFW_KEY_G;
        case core::Key::H: return GLFW_KEY_H;
        case core::Key::I: return GLFW_KEY_I;
        case core::Key::J: return GLFW_KEY_J;
        case core::Key::K: return GLFW_KEY_K;
        case core::Key::L: return GLFW_KEY_L;
        case core::Key::M: return GLFW_KEY_M;
        case core::Key::N: return GLFW_KEY_N;
        case core::Key::O: return GLFW_KEY_O;
        case core::Key::P: return GLFW_KEY_P;
        case core::Key::Q: return GLFW_KEY_Q;
        case core::Key::R: return GLFW_KEY_R;
        case core::Key::S: return GLFW_KEY_S;
        case core::Key::T: return GLFW_KEY_T;
        case core::Key::U: return GLFW_KEY_U;
        case core::Key::V: return GLFW_KEY_V;
        case core::Key::W: return GLFW_KEY_W;
        case core::Key::X: return GLFW_KEY_X;
        case core::Key::Y: return GLFW_KEY_Y;
        case core::Key::Z: return GLFW_KEY_Z;

        case core::Key::F1: return GLFW_KEY_F1;
        case core::Key::F2: return GLFW_KEY_F2;
        case core::Key::F3: return GLFW_KEY_F3;
        case core::Key::F4: return GLFW_KEY_F4;
        case core::Key::F5: return GLFW_KEY_F5;
        case core::Key::F6: return GLFW_KEY_F6;
        case core::Key::F7: return GLFW_KEY_F7;
        case core::Key::F8: return GLFW_KEY_F8;
        case core::Key::F9: return GLFW_KEY_F9;
        case core::Key::F10: return GLFW_KEY_F10;
        case core::Key::F11: return GLFW_KEY_F11;
        case core::Key::F12: return GLFW_KEY_F12;
        case core::Key::F13: return GLFW_KEY_F13;
        case core::Key::F14: return GLFW_KEY_F14;
        case core::Key::F15: return GLFW_KEY_F15;
        case core::Key::F16: return GLFW_KEY_F16;
        case core::Key::F17: return GLFW_KEY_F17;
        case core::Key::F18: return GLFW_KEY_F18;
        case core::Key::F19: return GLFW_KEY_F19;
        case core::Key::F20: return GLFW_KEY_F20;
        case core::Key::F21: return GLFW_KEY_F21;
        case core::Key::F22: return GLFW_KEY_F22;
        case core::Key::F23: return GLFW_KEY_F23;
        case core::Key::F24: return GLFW_KEY_F24;
        case core::Key::F25: return GLFW_KEY_F25;

        default: SIREN_ASSERT(false, "Invalid Siren core::KeyCode");
    }
}

/**
 * @brief Maps GLFW keycodes to siren.
 * @param key The GLFW keycode to map.
 * @return A siren @ref KeyCode.
 */
inline auto from_glfw_key(const u32 key) -> core::Key {
    switch (key) {
        case GLFW_KEY_SPACE: return core::Key::SPACE;
        case GLFW_KEY_APOSTROPHE: return core::Key::APOSTROPHE;
        case GLFW_KEY_COMMA: return core::Key::COMMA;
        case GLFW_KEY_MINUS: return core::Key::MINUS;
        case GLFW_KEY_PERIOD: return core::Key::PERIOD;
        case GLFW_KEY_SLASH: return core::Key::SLASH;
        case GLFW_KEY_0: return core::Key::NUM_0;
        case GLFW_KEY_1: return core::Key::NUM_1;
        case GLFW_KEY_2: return core::Key::NUM_2;
        case GLFW_KEY_3: return core::Key::NUM_3;
        case GLFW_KEY_4: return core::Key::NUM_4;
        case GLFW_KEY_5: return core::Key::NUM_5;
        case GLFW_KEY_6: return core::Key::NUM_6;
        case GLFW_KEY_7: return core::Key::NUM_7;
        case GLFW_KEY_8: return core::Key::NUM_8;
        case GLFW_KEY_9: return core::Key::NUM_9;
        case GLFW_KEY_SEMICOLON: return core::Key::SEMICOLON;
        case GLFW_KEY_EQUAL: return core::Key::EQUAL;
        case GLFW_KEY_LEFT_BRACKET: return core::Key::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return core::Key::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return core::Key::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return core::Key::GRAVE_ACCENT;
        case GLFW_KEY_ESCAPE: return core::Key::ESC;
        case GLFW_KEY_ENTER: return core::Key::ENTER;
        case GLFW_KEY_TAB: return core::Key::TAB;
        case GLFW_KEY_BACKSPACE: return core::Key::BACKSPACE;
        case GLFW_KEY_INSERT: return core::Key::INSERT;
        case GLFW_KEY_DELETE: return core::Key::DELETE_KEY;
        case GLFW_KEY_PAGE_UP: return core::Key::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return core::Key::PAGE_DOWN;
        case GLFW_KEY_HOME: return core::Key::HOME;
        case GLFW_KEY_END: return core::Key::END;
        case GLFW_KEY_CAPS_LOCK: return core::Key::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return core::Key::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return core::Key::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return core::Key::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return core::Key::PAUSE;
        case GLFW_KEY_KP_DECIMAL: return core::Key::KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return core::Key::KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return core::Key::KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return core::Key::KP_SUBTRACT;
        case GLFW_KEY_KP_ADD: return core::Key::KP_ADD;
        case GLFW_KEY_KP_ENTER: return core::Key::KP_ENTER;
        case GLFW_KEY_KP_EQUAL: return core::Key::KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return core::Key::L_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return core::Key::L_CONTROL;
        case GLFW_KEY_LEFT_ALT: return core::Key::L_ALT;
        case GLFW_KEY_LEFT_SUPER: return core::Key::L_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return core::Key::R_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return core::Key::R_CONTROL;
        case GLFW_KEY_RIGHT_ALT: return core::Key::R_ALT;
        case GLFW_KEY_RIGHT_SUPER: return core::Key::R_SUPER;
        case GLFW_KEY_MENU: return core::Key::MENU;
        case GLFW_KEY_LEFT: return core::Key::ARROW_LEFT;
        case GLFW_KEY_RIGHT: return core::Key::ARROW_RIGHT;
        case GLFW_KEY_UP: return core::Key::ARROW_UP;
        case GLFW_KEY_DOWN: return core::Key::ARROW_DOWN;

        case GLFW_KEY_A: return core::Key::A;
        case GLFW_KEY_B: return core::Key::B;
        case GLFW_KEY_C: return core::Key::C;
        case GLFW_KEY_D: return core::Key::D;
        case GLFW_KEY_E: return core::Key::E;
        case GLFW_KEY_F: return core::Key::F;
        case GLFW_KEY_G: return core::Key::G;
        case GLFW_KEY_H: return core::Key::H;
        case GLFW_KEY_I: return core::Key::I;
        case GLFW_KEY_J: return core::Key::J;
        case GLFW_KEY_K: return core::Key::K;
        case GLFW_KEY_L: return core::Key::L;
        case GLFW_KEY_M: return core::Key::M;
        case GLFW_KEY_N: return core::Key::N;
        case GLFW_KEY_O: return core::Key::O;
        case GLFW_KEY_P: return core::Key::P;
        case GLFW_KEY_Q: return core::Key::Q;
        case GLFW_KEY_R: return core::Key::R;
        case GLFW_KEY_S: return core::Key::S;
        case GLFW_KEY_T: return core::Key::T;
        case GLFW_KEY_U: return core::Key::U;
        case GLFW_KEY_V: return core::Key::V;
        case GLFW_KEY_W: return core::Key::W;
        case GLFW_KEY_X: return core::Key::X;
        case GLFW_KEY_Y: return core::Key::Y;
        case GLFW_KEY_Z: return core::Key::Z;

        case GLFW_KEY_F1: return core::Key::F1;
        case GLFW_KEY_F2: return core::Key::F2;
        case GLFW_KEY_F3: return core::Key::F3;
        case GLFW_KEY_F4: return core::Key::F4;
        case GLFW_KEY_F5: return core::Key::F5;
        case GLFW_KEY_F6: return core::Key::F6;
        case GLFW_KEY_F7: return core::Key::F7;
        case GLFW_KEY_F8: return core::Key::F8;
        case GLFW_KEY_F9: return core::Key::F9;
        case GLFW_KEY_F10: return core::Key::F10;
        case GLFW_KEY_F11: return core::Key::F11;
        case GLFW_KEY_F12: return core::Key::F12;
        case GLFW_KEY_F13: return core::Key::F13;
        case GLFW_KEY_F14: return core::Key::F14;
        case GLFW_KEY_F15: return core::Key::F15;
        case GLFW_KEY_F16: return core::Key::F16;
        case GLFW_KEY_F17: return core::Key::F17;
        case GLFW_KEY_F18: return core::Key::F18;
        case GLFW_KEY_F19: return core::Key::F19;
        case GLFW_KEY_F20: return core::Key::F20;
        case GLFW_KEY_F21: return core::Key::F21;
        case GLFW_KEY_F22: return core::Key::F22;
        case GLFW_KEY_F23: return core::Key::F23;
        case GLFW_KEY_F24: return core::Key::F24;
        case GLFW_KEY_F25: return core::Key::F25;

        default: SIREN_ASSERT(false, "Invalid GLFW core::KeyCode");
    }
}

/**
 * @brief Maps siren MouseCode's to native GLFW.
 * @param key The siren @ref MouseCode to map.
 * @return An unsigned integer representing a GLFW mouse key.
 */
inline auto to_glfw(const core::Mouse key) -> u32 {
    switch (key) {
        case core::Mouse::Left: return GLFW_MOUSE_BUTTON_LEFT;
        case core::Mouse::Right: return GLFW_MOUSE_BUTTON_RIGHT;
        case core::Mouse::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        default: SIREN_ASSERT(false, "Invalid Siren core::MouseCode");
    }
}

/**
 * @brief Maps GLFW mouse keycodes to siren.
 * @param key The GLFW keycode to map.
 * @return A siren @ref MouseCode.
 */
inline auto from_glfw_mouse(const u32 key) -> core::Mouse {
    switch (key) {
        case GLFW_MOUSE_BUTTON_LEFT: return core::Mouse::Left;
        case GLFW_MOUSE_BUTTON_RIGHT: return core::Mouse::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE: return core::Mouse::Middle;
        default: SIREN_ASSERT(false, "Invalid GLFW core::MouseCode");
    }
}

/**
 * @brief Maps siren MouseMode's to native GLFW.
 * @param mode The siren @ref MouseMode to map.
 * @return An unsigned integer representing a GLFW mouse mode.
 */
inline auto to_glfw(const core::CursorMode mode) -> u32 {
    switch (mode) {
        case core::CursorMode::Visible: return GLFW_CURSOR_NORMAL;
        case core::CursorMode::Invisible: return GLFW_CURSOR_HIDDEN;
        case core::CursorMode::Locked: return GLFW_CURSOR_DISABLED;
        default: return GLFW_CURSOR_NORMAL;
    }
}

/**
 * @brief Maps GLFW mouse keycodes to siren.
 * @param mode The GLFW mouse mode to map.
 * @return A siren @ref MouseMode.
 */
inline auto from_glfw_mouse_mode(const u32 mode) -> core::CursorMode {
    switch (mode) {
        case GLFW_CURSOR_NORMAL: return core::CursorMode::Visible;
        case GLFW_CURSOR_HIDDEN: return core::CursorMode::Invisible;
        case GLFW_CURSOR_DISABLED: return core::CursorMode::Locked;
        default: return core::CursorMode::Invalid;
    }
}
} // namespace siren::platform

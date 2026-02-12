#pragma once

#include <GLFW/glfw3.h>
#include "input/input_codes.hpp"
#include "utilities/spch.hpp"


namespace siren::platform
{
inline i32 toGLFW(const core::KeyCode key)
{
    switch (key) {
        case core::KeyCode::SPACE: return GLFW_KEY_SPACE;
        case core::KeyCode::APOSTROPHE: return GLFW_KEY_APOSTROPHE;
        case core::KeyCode::COMMA: return GLFW_KEY_COMMA;
        case core::KeyCode::MINUS: return GLFW_KEY_MINUS;
        case core::KeyCode::PERIOD: return GLFW_KEY_PERIOD;
        case core::KeyCode::SLASH: return GLFW_KEY_SLASH;
        case core::KeyCode::NUM_0: return GLFW_KEY_0;
        case core::KeyCode::NUM_1: return GLFW_KEY_1;
        case core::KeyCode::NUM_2: return GLFW_KEY_2;
        case core::KeyCode::NUM_3: return GLFW_KEY_3;
        case core::KeyCode::NUM_4: return GLFW_KEY_4;
        case core::KeyCode::NUM_5: return GLFW_KEY_5;
        case core::KeyCode::NUM_6: return GLFW_KEY_6;
        case core::KeyCode::NUM_7: return GLFW_KEY_7;
        case core::KeyCode::NUM_8: return GLFW_KEY_8;
        case core::KeyCode::NUM_9: return GLFW_KEY_9;
        case core::KeyCode::SEMICOLON: return GLFW_KEY_SEMICOLON;
        case core::KeyCode::EQUAL: return GLFW_KEY_EQUAL;
        case core::KeyCode::LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
        case core::KeyCode::BACKSLASH: return GLFW_KEY_BACKSLASH;
        case core::KeyCode::RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;
        case core::KeyCode::GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;
        case core::KeyCode::ESC: return GLFW_KEY_ESCAPE;
        case core::KeyCode::ENTER: return GLFW_KEY_ENTER;
        case core::KeyCode::TAB: return GLFW_KEY_TAB;
        case core::KeyCode::BACKSPACE: return GLFW_KEY_BACKSPACE;
        case core::KeyCode::INSERT: return GLFW_KEY_INSERT;
        case core::KeyCode::DELETE_KEY: return GLFW_KEY_DELETE;
        case core::KeyCode::PAGE_UP: return GLFW_KEY_PAGE_UP;
        case core::KeyCode::PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
        case core::KeyCode::HOME: return GLFW_KEY_HOME;
        case core::KeyCode::END: return GLFW_KEY_END;
        case core::KeyCode::CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
        case core::KeyCode::SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
        case core::KeyCode::NUM_LOCK: return GLFW_KEY_NUM_LOCK;
        case core::KeyCode::PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
        case core::KeyCode::PAUSE: return GLFW_KEY_PAUSE;
        case core::KeyCode::KP_DECIMAL: return GLFW_KEY_KP_DECIMAL;
        case core::KeyCode::KP_DIVIDE: return GLFW_KEY_KP_DIVIDE;
        case core::KeyCode::KP_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
        case core::KeyCode::KP_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
        case core::KeyCode::KP_ADD: return GLFW_KEY_KP_ADD;
        case core::KeyCode::KP_ENTER: return GLFW_KEY_KP_ENTER;
        case core::KeyCode::KP_EQUAL: return GLFW_KEY_KP_EQUAL;
        case core::KeyCode::L_SHIFT: return GLFW_KEY_LEFT_SHIFT;
        case core::KeyCode::L_CONTROL: return GLFW_KEY_LEFT_CONTROL;
        case core::KeyCode::L_ALT: return GLFW_KEY_LEFT_ALT;
        case core::KeyCode::L_SUPER: return GLFW_KEY_LEFT_SUPER;
        case core::KeyCode::R_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
        case core::KeyCode::R_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
        case core::KeyCode::R_ALT: return GLFW_KEY_RIGHT_ALT;
        case core::KeyCode::R_SUPER: return GLFW_KEY_RIGHT_SUPER;
        case core::KeyCode::MENU: return GLFW_KEY_MENU;
        case core::KeyCode::KEY_LAST: return GLFW_KEY_LAST;
        case core::KeyCode::ARROW_LEFT: return GLFW_KEY_LEFT;
        case core::KeyCode::ARROW_RIGHT: return GLFW_KEY_RIGHT;
        case core::KeyCode::ARROW_UP: return GLFW_KEY_UP;
        case core::KeyCode::ARROW_DOWN: return GLFW_KEY_DOWN;

        case core::KeyCode::A: return GLFW_KEY_A;
        case core::KeyCode::B: return GLFW_KEY_B;
        case core::KeyCode::C: return GLFW_KEY_C;
        case core::KeyCode::D: return GLFW_KEY_D;
        case core::KeyCode::E: return GLFW_KEY_E;
        case core::KeyCode::F: return GLFW_KEY_F;
        case core::KeyCode::G: return GLFW_KEY_G;
        case core::KeyCode::H: return GLFW_KEY_H;
        case core::KeyCode::I: return GLFW_KEY_I;
        case core::KeyCode::J: return GLFW_KEY_J;
        case core::KeyCode::K: return GLFW_KEY_K;
        case core::KeyCode::L: return GLFW_KEY_L;
        case core::KeyCode::M: return GLFW_KEY_M;
        case core::KeyCode::N: return GLFW_KEY_N;
        case core::KeyCode::O: return GLFW_KEY_O;
        case core::KeyCode::P: return GLFW_KEY_P;
        case core::KeyCode::Q: return GLFW_KEY_Q;
        case core::KeyCode::R: return GLFW_KEY_R;
        case core::KeyCode::S: return GLFW_KEY_S;
        case core::KeyCode::T: return GLFW_KEY_T;
        case core::KeyCode::U: return GLFW_KEY_U;
        case core::KeyCode::V: return GLFW_KEY_V;
        case core::KeyCode::W: return GLFW_KEY_W;
        case core::KeyCode::X: return GLFW_KEY_X;
        case core::KeyCode::Y: return GLFW_KEY_Y;
        case core::KeyCode::Z: return GLFW_KEY_Z;

        case core::KeyCode::F1: return GLFW_KEY_F1;
        case core::KeyCode::F2: return GLFW_KEY_F2;
        case core::KeyCode::F3: return GLFW_KEY_F3;
        case core::KeyCode::F4: return GLFW_KEY_F4;
        case core::KeyCode::F5: return GLFW_KEY_F5;
        case core::KeyCode::F6: return GLFW_KEY_F6;
        case core::KeyCode::F7: return GLFW_KEY_F7;
        case core::KeyCode::F8: return GLFW_KEY_F8;
        case core::KeyCode::F9: return GLFW_KEY_F9;
        case core::KeyCode::F10: return GLFW_KEY_F10;
        case core::KeyCode::F11: return GLFW_KEY_F11;
        case core::KeyCode::F12: return GLFW_KEY_F12;
        case core::KeyCode::F13: return GLFW_KEY_F13;
        case core::KeyCode::F14: return GLFW_KEY_F14;
        case core::KeyCode::F15: return GLFW_KEY_F15;
        case core::KeyCode::F16: return GLFW_KEY_F16;
        case core::KeyCode::F17: return GLFW_KEY_F17;
        case core::KeyCode::F18: return GLFW_KEY_F18;
        case core::KeyCode::F19: return GLFW_KEY_F19;
        case core::KeyCode::F20: return GLFW_KEY_F20;
        case core::KeyCode::F21: return GLFW_KEY_F21;
        case core::KeyCode::F22: return GLFW_KEY_F22;
        case core::KeyCode::F23: return GLFW_KEY_F23;
        case core::KeyCode::F24: return GLFW_KEY_F24;
        case core::KeyCode::F25: return GLFW_KEY_F25;

        default: SIREN_ASSERT(false, "Invalid Siren core::KeyCode");
    }
}

inline core::KeyCode fromGLFW(const i32 glfwKey)
{
    switch (glfwKey) {
        case GLFW_KEY_SPACE: return core::KeyCode::SPACE;
        case GLFW_KEY_APOSTROPHE: return core::KeyCode::APOSTROPHE;
        case GLFW_KEY_COMMA: return core::KeyCode::COMMA;
        case GLFW_KEY_MINUS: return core::KeyCode::MINUS;
        case GLFW_KEY_PERIOD: return core::KeyCode::PERIOD;
        case GLFW_KEY_SLASH: return core::KeyCode::SLASH;
        case GLFW_KEY_0: return core::KeyCode::NUM_0;
        case GLFW_KEY_1: return core::KeyCode::NUM_1;
        case GLFW_KEY_2: return core::KeyCode::NUM_2;
        case GLFW_KEY_3: return core::KeyCode::NUM_3;
        case GLFW_KEY_4: return core::KeyCode::NUM_4;
        case GLFW_KEY_5: return core::KeyCode::NUM_5;
        case GLFW_KEY_6: return core::KeyCode::NUM_6;
        case GLFW_KEY_7: return core::KeyCode::NUM_7;
        case GLFW_KEY_8: return core::KeyCode::NUM_8;
        case GLFW_KEY_9: return core::KeyCode::NUM_9;
        case GLFW_KEY_SEMICOLON: return core::KeyCode::SEMICOLON;
        case GLFW_KEY_EQUAL: return core::KeyCode::EQUAL;
        case GLFW_KEY_LEFT_BRACKET: return core::KeyCode::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return core::KeyCode::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return core::KeyCode::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return core::KeyCode::GRAVE_ACCENT;
        case GLFW_KEY_ESCAPE: return core::KeyCode::ESC;
        case GLFW_KEY_ENTER: return core::KeyCode::ENTER;
        case GLFW_KEY_TAB: return core::KeyCode::TAB;
        case GLFW_KEY_BACKSPACE: return core::KeyCode::BACKSPACE;
        case GLFW_KEY_INSERT: return core::KeyCode::INSERT;
        case GLFW_KEY_DELETE: return core::KeyCode::DELETE_KEY;
        case GLFW_KEY_PAGE_UP: return core::KeyCode::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return core::KeyCode::PAGE_DOWN;
        case GLFW_KEY_HOME: return core::KeyCode::HOME;
        case GLFW_KEY_END: return core::KeyCode::END;
        case GLFW_KEY_CAPS_LOCK: return core::KeyCode::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return core::KeyCode::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return core::KeyCode::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return core::KeyCode::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return core::KeyCode::PAUSE;
        case GLFW_KEY_KP_DECIMAL: return core::KeyCode::KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return core::KeyCode::KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return core::KeyCode::KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return core::KeyCode::KP_SUBTRACT;
        case GLFW_KEY_KP_ADD: return core::KeyCode::KP_ADD;
        case GLFW_KEY_KP_ENTER: return core::KeyCode::KP_ENTER;
        case GLFW_KEY_KP_EQUAL: return core::KeyCode::KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return core::KeyCode::L_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return core::KeyCode::L_CONTROL;
        case GLFW_KEY_LEFT_ALT: return core::KeyCode::L_ALT;
        case GLFW_KEY_LEFT_SUPER: return core::KeyCode::L_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return core::KeyCode::R_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return core::KeyCode::R_CONTROL;
        case GLFW_KEY_RIGHT_ALT: return core::KeyCode::R_ALT;
        case GLFW_KEY_RIGHT_SUPER: return core::KeyCode::R_SUPER;
        case GLFW_KEY_MENU: return core::KeyCode::MENU;
        case GLFW_KEY_LEFT: return core::KeyCode::ARROW_LEFT;
        case GLFW_KEY_RIGHT: return core::KeyCode::ARROW_RIGHT;
        case GLFW_KEY_UP: return core::KeyCode::ARROW_UP;
        case GLFW_KEY_DOWN: return core::KeyCode::ARROW_DOWN;

        case GLFW_KEY_A: return core::KeyCode::A;
        case GLFW_KEY_B: return core::KeyCode::B;
        case GLFW_KEY_C: return core::KeyCode::C;
        case GLFW_KEY_D: return core::KeyCode::D;
        case GLFW_KEY_E: return core::KeyCode::E;
        case GLFW_KEY_F: return core::KeyCode::F;
        case GLFW_KEY_G: return core::KeyCode::G;
        case GLFW_KEY_H: return core::KeyCode::H;
        case GLFW_KEY_I: return core::KeyCode::I;
        case GLFW_KEY_J: return core::KeyCode::J;
        case GLFW_KEY_K: return core::KeyCode::K;
        case GLFW_KEY_L: return core::KeyCode::L;
        case GLFW_KEY_M: return core::KeyCode::M;
        case GLFW_KEY_N: return core::KeyCode::N;
        case GLFW_KEY_O: return core::KeyCode::O;
        case GLFW_KEY_P: return core::KeyCode::P;
        case GLFW_KEY_Q: return core::KeyCode::Q;
        case GLFW_KEY_R: return core::KeyCode::R;
        case GLFW_KEY_S: return core::KeyCode::S;
        case GLFW_KEY_T: return core::KeyCode::T;
        case GLFW_KEY_U: return core::KeyCode::U;
        case GLFW_KEY_V: return core::KeyCode::V;
        case GLFW_KEY_W: return core::KeyCode::W;
        case GLFW_KEY_X: return core::KeyCode::X;
        case GLFW_KEY_Y: return core::KeyCode::Y;
        case GLFW_KEY_Z: return core::KeyCode::Z;

        case GLFW_KEY_F1: return core::KeyCode::F1;
        case GLFW_KEY_F2: return core::KeyCode::F2;
        case GLFW_KEY_F3: return core::KeyCode::F3;
        case GLFW_KEY_F4: return core::KeyCode::F4;
        case GLFW_KEY_F5: return core::KeyCode::F5;
        case GLFW_KEY_F6: return core::KeyCode::F6;
        case GLFW_KEY_F7: return core::KeyCode::F7;
        case GLFW_KEY_F8: return core::KeyCode::F8;
        case GLFW_KEY_F9: return core::KeyCode::F9;
        case GLFW_KEY_F10: return core::KeyCode::F10;
        case GLFW_KEY_F11: return core::KeyCode::F11;
        case GLFW_KEY_F12: return core::KeyCode::F12;
        case GLFW_KEY_F13: return core::KeyCode::F13;
        case GLFW_KEY_F14: return core::KeyCode::F14;
        case GLFW_KEY_F15: return core::KeyCode::F15;
        case GLFW_KEY_F16: return core::KeyCode::F16;
        case GLFW_KEY_F17: return core::KeyCode::F17;
        case GLFW_KEY_F18: return core::KeyCode::F18;
        case GLFW_KEY_F19: return core::KeyCode::F19;
        case GLFW_KEY_F20: return core::KeyCode::F20;
        case GLFW_KEY_F21: return core::KeyCode::F21;
        case GLFW_KEY_F22: return core::KeyCode::F22;
        case GLFW_KEY_F23: return core::KeyCode::F23;
        case GLFW_KEY_F24: return core::KeyCode::F24;
        case GLFW_KEY_F25: return core::KeyCode::F25;

        default: SIREN_ASSERT(false, "Invalid GLFW core::KeyCode");
    }
}

inline i32 toGLFW(const core::MouseCode btn)
{
    switch (btn) {
        case core::MouseCode::Left: return GLFW_MOUSE_BUTTON_LEFT;
        case core::MouseCode::Right: return GLFW_MOUSE_BUTTON_RIGHT;
        case core::MouseCode::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        default: SIREN_ASSERT(false, "Invalid Siren core::MouseCode");
    }
}

inline core::MouseCode fromGLFWMouse(const i32 btn)
{
    switch (btn) {
        case GLFW_MOUSE_BUTTON_LEFT: return core::MouseCode::Left;
        case GLFW_MOUSE_BUTTON_RIGHT: return core::MouseCode::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE: return core::MouseCode::Middle;
        default: SIREN_ASSERT(false, "Invalid GLFW core::MouseCode");
    }
}

inline i32 toGLFWMouseMode(const core::MouseMode mode)
{
    switch (mode) {
        case core::MouseMode::Visible: return GLFW_CURSOR_NORMAL;
        case core::MouseMode::Invisible: return GLFW_CURSOR_HIDDEN;
        case core::MouseMode::Locked: return GLFW_CURSOR_DISABLED;
        default: return GLFW_CURSOR_NORMAL;
    }
}

inline core::MouseMode fromGLFWMouseMode(const i32 glfwMode)
{
    switch (glfwMode) {
        case GLFW_CURSOR_NORMAL: return core::MouseMode::Visible;
        case GLFW_CURSOR_HIDDEN: return core::MouseMode::Invisible;
        case GLFW_CURSOR_DISABLED: return core::MouseMode::Locked;
        default: return core::MouseMode::Invalid;
    }
}
} // namespace siren::platform

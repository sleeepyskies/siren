#include "WindowsInput.hpp"

#include "platform/GL.hpp"
#include "utilities/spch.hpp"


namespace siren::platform
{
using namespace siren::core;
// ============================================================================
// == Mark: glfw to Siren input codes ==
// ============================================================================
static int toGLFW(const KeyCode key)
{
    switch (key) {
        case KeyCode::SPACE: return GLFW_KEY_SPACE;
        case KeyCode::APOSTROPHE: return GLFW_KEY_APOSTROPHE;
        case KeyCode::COMMA: return GLFW_KEY_COMMA;
        case KeyCode::MINUS: return GLFW_KEY_MINUS;
        case KeyCode::PERIOD: return GLFW_KEY_PERIOD;
        case KeyCode::SLASH: return GLFW_KEY_SLASH;
        case KeyCode::NUM_0: return GLFW_KEY_0;
        case KeyCode::NUM_1: return GLFW_KEY_1;
        case KeyCode::NUM_2: return GLFW_KEY_2;
        case KeyCode::NUM_3: return GLFW_KEY_3;
        case KeyCode::NUM_4: return GLFW_KEY_4;
        case KeyCode::NUM_5: return GLFW_KEY_5;
        case KeyCode::NUM_6: return GLFW_KEY_6;
        case KeyCode::NUM_7: return GLFW_KEY_7;
        case KeyCode::NUM_8: return GLFW_KEY_8;
        case KeyCode::NUM_9: return GLFW_KEY_9;
        case KeyCode::SEMICOLON: return GLFW_KEY_SEMICOLON;
        case KeyCode::EQUAL: return GLFW_KEY_EQUAL;
        case KeyCode::LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
        case KeyCode::BACKSLASH: return GLFW_KEY_BACKSLASH;
        case KeyCode::RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;
        case KeyCode::GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;
        case KeyCode::ESC: return GLFW_KEY_ESCAPE;
        case KeyCode::ENTER: return GLFW_KEY_ENTER;
        case KeyCode::TAB: return GLFW_KEY_TAB;
        case KeyCode::BACKSPACE: return GLFW_KEY_BACKSPACE;
        case KeyCode::INSERT: return GLFW_KEY_INSERT;
        case KeyCode::DELETE_KEY: return GLFW_KEY_DELETE;
        case KeyCode::PAGE_UP: return GLFW_KEY_PAGE_UP;
        case KeyCode::PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
        case KeyCode::HOME: return GLFW_KEY_HOME;
        case KeyCode::END: return GLFW_KEY_END;
        case KeyCode::CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
        case KeyCode::SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
        case KeyCode::NUM_LOCK: return GLFW_KEY_NUM_LOCK;
        case KeyCode::PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
        case KeyCode::PAUSE: return GLFW_KEY_PAUSE;
        case KeyCode::KP_DECIMAL: return GLFW_KEY_KP_DECIMAL;
        case KeyCode::KP_DIVIDE: return GLFW_KEY_KP_DIVIDE;
        case KeyCode::KP_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
        case KeyCode::KP_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
        case KeyCode::KP_ADD: return GLFW_KEY_KP_ADD;
        case KeyCode::KP_ENTER: return GLFW_KEY_KP_ENTER;
        case KeyCode::KP_EQUAL: return GLFW_KEY_KP_EQUAL;
        case KeyCode::L_SHIFT: return GLFW_KEY_LEFT_SHIFT;
        case KeyCode::L_CONTROL: return GLFW_KEY_LEFT_CONTROL;
        case KeyCode::L_ALT: return GLFW_KEY_LEFT_ALT;
        case KeyCode::L_SUPER: return GLFW_KEY_LEFT_SUPER;
        case KeyCode::R_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
        case KeyCode::R_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
        case KeyCode::R_ALT: return GLFW_KEY_RIGHT_ALT;
        case KeyCode::R_SUPER: return GLFW_KEY_RIGHT_SUPER;
        case KeyCode::MENU: return GLFW_KEY_MENU;
        case KeyCode::KEY_LAST: return GLFW_KEY_LAST;
        case KeyCode::ARROW_LEFT: return GLFW_KEY_LEFT;
        case KeyCode::ARROW_RIGHT: return GLFW_KEY_RIGHT;
        case KeyCode::ARROW_UP: return GLFW_KEY_UP;
        case KeyCode::ARROW_DOWN: return GLFW_KEY_DOWN;

        case KeyCode::A: return GLFW_KEY_A;
        case KeyCode::B: return GLFW_KEY_B;
        case KeyCode::C: return GLFW_KEY_C;
        case KeyCode::D: return GLFW_KEY_D;
        case KeyCode::E: return GLFW_KEY_E;
        case KeyCode::F: return GLFW_KEY_F;
        case KeyCode::G: return GLFW_KEY_G;
        case KeyCode::H: return GLFW_KEY_H;
        case KeyCode::I: return GLFW_KEY_I;
        case KeyCode::J: return GLFW_KEY_J;
        case KeyCode::K: return GLFW_KEY_K;
        case KeyCode::L: return GLFW_KEY_L;
        case KeyCode::M: return GLFW_KEY_M;
        case KeyCode::N: return GLFW_KEY_N;
        case KeyCode::O: return GLFW_KEY_O;
        case KeyCode::P: return GLFW_KEY_P;
        case KeyCode::Q: return GLFW_KEY_Q;
        case KeyCode::R: return GLFW_KEY_R;
        case KeyCode::S: return GLFW_KEY_S;
        case KeyCode::T: return GLFW_KEY_T;
        case KeyCode::U: return GLFW_KEY_U;
        case KeyCode::V: return GLFW_KEY_V;
        case KeyCode::W: return GLFW_KEY_W;
        case KeyCode::X: return GLFW_KEY_X;
        case KeyCode::Y: return GLFW_KEY_Y;
        case KeyCode::Z: return GLFW_KEY_Z;

        case KeyCode::F1: return GLFW_KEY_F1;
        case KeyCode::F2: return GLFW_KEY_F2;
        case KeyCode::F3: return GLFW_KEY_F3;
        case KeyCode::F4: return GLFW_KEY_F4;
        case KeyCode::F5: return GLFW_KEY_F5;
        case KeyCode::F6: return GLFW_KEY_F6;
        case KeyCode::F7: return GLFW_KEY_F7;
        case KeyCode::F8: return GLFW_KEY_F8;
        case KeyCode::F9: return GLFW_KEY_F9;
        case KeyCode::F10: return GLFW_KEY_F10;
        case KeyCode::F11: return GLFW_KEY_F11;
        case KeyCode::F12: return GLFW_KEY_F12;
        case KeyCode::F13: return GLFW_KEY_F13;
        case KeyCode::F14: return GLFW_KEY_F14;
        case KeyCode::F15: return GLFW_KEY_F15;
        case KeyCode::F16: return GLFW_KEY_F16;
        case KeyCode::F17: return GLFW_KEY_F17;
        case KeyCode::F18: return GLFW_KEY_F18;
        case KeyCode::F19: return GLFW_KEY_F19;
        case KeyCode::F20: return GLFW_KEY_F20;
        case KeyCode::F21: return GLFW_KEY_F21;
        case KeyCode::F22: return GLFW_KEY_F22;
        case KeyCode::F23: return GLFW_KEY_F23;
        case KeyCode::F24: return GLFW_KEY_F24;
        case KeyCode::F25: return GLFW_KEY_F25;

        default: SirenAssert(false, "Invalid Siren KeyCode");
    }
}

static KeyCode fromGLFW(const i32 glfwKey)
{
    switch (glfwKey) {
        case GLFW_KEY_SPACE: return KeyCode::SPACE;
        case GLFW_KEY_APOSTROPHE: return KeyCode::APOSTROPHE;
        case GLFW_KEY_COMMA: return KeyCode::COMMA;
        case GLFW_KEY_MINUS: return KeyCode::MINUS;
        case GLFW_KEY_PERIOD: return KeyCode::PERIOD;
        case GLFW_KEY_SLASH: return KeyCode::SLASH;
        case GLFW_KEY_0: return KeyCode::NUM_0;
        case GLFW_KEY_1: return KeyCode::NUM_1;
        case GLFW_KEY_2: return KeyCode::NUM_2;
        case GLFW_KEY_3: return KeyCode::NUM_3;
        case GLFW_KEY_4: return KeyCode::NUM_4;
        case GLFW_KEY_5: return KeyCode::NUM_5;
        case GLFW_KEY_6: return KeyCode::NUM_6;
        case GLFW_KEY_7: return KeyCode::NUM_7;
        case GLFW_KEY_8: return KeyCode::NUM_8;
        case GLFW_KEY_9: return KeyCode::NUM_9;
        case GLFW_KEY_SEMICOLON: return KeyCode::SEMICOLON;
        case GLFW_KEY_EQUAL: return KeyCode::EQUAL;
        case GLFW_KEY_LEFT_BRACKET: return KeyCode::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return KeyCode::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return KeyCode::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return KeyCode::GRAVE_ACCENT;
        case GLFW_KEY_ESCAPE: return KeyCode::ESC;
        case GLFW_KEY_ENTER: return KeyCode::ENTER;
        case GLFW_KEY_TAB: return KeyCode::TAB;
        case GLFW_KEY_BACKSPACE: return KeyCode::BACKSPACE;
        case GLFW_KEY_INSERT: return KeyCode::INSERT;
        case GLFW_KEY_DELETE: return KeyCode::DELETE_KEY;
        case GLFW_KEY_PAGE_UP: return KeyCode::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return KeyCode::PAGE_DOWN;
        case GLFW_KEY_HOME: return KeyCode::HOME;
        case GLFW_KEY_END: return KeyCode::END;
        case GLFW_KEY_CAPS_LOCK: return KeyCode::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return KeyCode::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return KeyCode::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return KeyCode::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return KeyCode::PAUSE;
        case GLFW_KEY_KP_DECIMAL: return KeyCode::KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return KeyCode::KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return KeyCode::KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return KeyCode::KP_SUBTRACT;
        case GLFW_KEY_KP_ADD: return KeyCode::KP_ADD;
        case GLFW_KEY_KP_ENTER: return KeyCode::KP_ENTER;
        case GLFW_KEY_KP_EQUAL: return KeyCode::KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return KeyCode::L_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return KeyCode::L_CONTROL;
        case GLFW_KEY_LEFT_ALT: return KeyCode::L_ALT;
        case GLFW_KEY_LEFT_SUPER: return KeyCode::L_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return KeyCode::R_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return KeyCode::R_CONTROL;
        case GLFW_KEY_RIGHT_ALT: return KeyCode::R_ALT;
        case GLFW_KEY_RIGHT_SUPER: return KeyCode::R_SUPER;
        case GLFW_KEY_MENU: return KeyCode::MENU;
        case GLFW_KEY_LEFT: return KeyCode::ARROW_LEFT;
        case GLFW_KEY_RIGHT: return KeyCode::ARROW_RIGHT;
        case GLFW_KEY_UP: return KeyCode::ARROW_UP;
        case GLFW_KEY_DOWN: return KeyCode::ARROW_DOWN;

        case GLFW_KEY_A: return KeyCode::A;
        case GLFW_KEY_B: return KeyCode::B;
        case GLFW_KEY_C: return KeyCode::C;
        case GLFW_KEY_D: return KeyCode::D;
        case GLFW_KEY_E: return KeyCode::E;
        case GLFW_KEY_F: return KeyCode::F;
        case GLFW_KEY_G: return KeyCode::G;
        case GLFW_KEY_H: return KeyCode::H;
        case GLFW_KEY_I: return KeyCode::I;
        case GLFW_KEY_J: return KeyCode::J;
        case GLFW_KEY_K: return KeyCode::K;
        case GLFW_KEY_L: return KeyCode::L;
        case GLFW_KEY_M: return KeyCode::M;
        case GLFW_KEY_N: return KeyCode::N;
        case GLFW_KEY_O: return KeyCode::O;
        case GLFW_KEY_P: return KeyCode::P;
        case GLFW_KEY_Q: return KeyCode::Q;
        case GLFW_KEY_R: return KeyCode::R;
        case GLFW_KEY_S: return KeyCode::S;
        case GLFW_KEY_T: return KeyCode::T;
        case GLFW_KEY_U: return KeyCode::U;
        case GLFW_KEY_V: return KeyCode::V;
        case GLFW_KEY_W: return KeyCode::W;
        case GLFW_KEY_X: return KeyCode::X;
        case GLFW_KEY_Y: return KeyCode::Y;
        case GLFW_KEY_Z: return KeyCode::Z;

        case GLFW_KEY_F1: return KeyCode::F1;
        case GLFW_KEY_F2: return KeyCode::F2;
        case GLFW_KEY_F3: return KeyCode::F3;
        case GLFW_KEY_F4: return KeyCode::F4;
        case GLFW_KEY_F5: return KeyCode::F5;
        case GLFW_KEY_F6: return KeyCode::F6;
        case GLFW_KEY_F7: return KeyCode::F7;
        case GLFW_KEY_F8: return KeyCode::F8;
        case GLFW_KEY_F9: return KeyCode::F9;
        case GLFW_KEY_F10: return KeyCode::F10;
        case GLFW_KEY_F11: return KeyCode::F11;
        case GLFW_KEY_F12: return KeyCode::F12;
        case GLFW_KEY_F13: return KeyCode::F13;
        case GLFW_KEY_F14: return KeyCode::F14;
        case GLFW_KEY_F15: return KeyCode::F15;
        case GLFW_KEY_F16: return KeyCode::F16;
        case GLFW_KEY_F17: return KeyCode::F17;
        case GLFW_KEY_F18: return KeyCode::F18;
        case GLFW_KEY_F19: return KeyCode::F19;
        case GLFW_KEY_F20: return KeyCode::F20;
        case GLFW_KEY_F21: return KeyCode::F21;
        case GLFW_KEY_F22: return KeyCode::F22;
        case GLFW_KEY_F23: return KeyCode::F23;
        case GLFW_KEY_F24: return KeyCode::F24;
        case GLFW_KEY_F25: return KeyCode::F25;

        default: SirenAssert(false, "Invalid GLFW KeyCode");
    }
}

static int toGLFW(const MouseCode btn)
{
    switch (btn) {
        case MouseCode::LEFT: return GLFW_MOUSE_BUTTON_LEFT;
        case MouseCode::RIGHT: return GLFW_MOUSE_BUTTON_RIGHT;
        case MouseCode::MIDDLE: return GLFW_MOUSE_BUTTON_MIDDLE;
        default: SirenAssert(false, "Invalid Siren MouseCode");
    }
}

static MouseCode fromGLFWMouse(const i32 btn)
{
    switch (btn) {
        case GLFW_MOUSE_BUTTON_LEFT: return MouseCode::LEFT;
        case GLFW_MOUSE_BUTTON_RIGHT: return MouseCode::RIGHT;
        case GLFW_MOUSE_BUTTON_MIDDLE: return MouseCode::MIDDLE;
        default: SirenAssert(false, "Invalid GLFW MouseCode");
    }
}

static int toGLFWMouseMode(const MouseMode mode)
{
    switch (mode) {
        case MouseMode::VISIBLE: return GLFW_CURSOR_NORMAL;
        case MouseMode::INVISIBLE: return GLFW_CURSOR_HIDDEN;
        case MouseMode::LOCKED: return GLFW_CURSOR_DISABLED;
        default: return GLFW_CURSOR_NORMAL;
    }
}

static MouseMode fromGLFWMouseMode(const i32 glfwMode)
{
    switch (glfwMode) {
        case GLFW_CURSOR_NORMAL: return MouseMode::VISIBLE;
        case GLFW_CURSOR_HIDDEN: return MouseMode::INVISIBLE;
        case GLFW_CURSOR_DISABLED: return MouseMode::LOCKED;
        default: return MouseMode::INVALID;
    }
}

// ============================================================================
// == Mark: Input Functions ==
// ============================================================================

WindowsInput::WindowsInput(GLFWwindow* window) : m_window(window) { }

bool WindowsInput::isKeyHeld(const KeyCode code)
{
    return glfwGetKey(m_window, toGLFW(code)) == GLFW_PRESS;
}

bool WindowsInput::isMouseKeyHeld(const MouseCode code)
{
    return glfwGetMouseButton(m_window, toGLFW(code)) == GLFW_PRESS;
}

glm::vec2 WindowsInput::getMousePosition()
{
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return { x, y };
}

void WindowsInput::setMousePosition(const glm::vec2 position)
{
    glfwSetCursorPos(m_window, position.x, position.y);
}

MouseMode WindowsInput::getMouseMode()
{
    const int glfwMode = glfwGetInputMode(m_window, GLFW_CURSOR);

    return fromGLFWMouseMode(glfwMode);
}

void WindowsInput::setMouseMode(const MouseMode mode)
{
    glfwSetInputMode(m_window, GLFW_CURSOR, toGLFWMouseMode(mode));
}
} // namespace siren::platform

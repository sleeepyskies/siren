#include "WindowsInput.hpp"

#include "platform/GL.hpp"
#include "utilities/spch.hpp"

namespace siren::platform
{

WindowsInput::WindowsInput(GLFWwindow* window) : m_window(window) {}

bool WindowsInput::isKeyPressed(core::KeyCode code)
{
    return glfwGetKey(m_window, static_cast<int>(code)) == GLFW_PRESS;
}

bool WindowsInput::isMouseKeyPressed(core::MouseCode code)
{
    return glfwGetMouseButton(m_window, static_cast<int>(code)) == GLFW_PRESS;
}

glm::vec2 WindowsInput::getMousePosition()
{
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return {x, y};
}

void WindowsInput::setMousePosition(const glm::vec2 position)
{
    glfwSetCursorPos(m_window, position.x, position.y);
}

core::MouseMode WindowsInput::getMouseMode()
{
    const int glfwMode = glfwGetInputMode(m_window, GLFW_CURSOR);

    switch (glfwMode) {
        case GLFW_CURSOR_NORMAL: return core::MouseMode::VISIBLE;
        case GLFW_CURSOR_HIDDEN: return core::MouseMode::INVISIBLE;
        case GLFW_CURSOR_DISABLED: return core::MouseMode::LOCKED;
        default: return core::MouseMode::INVALID;
    }
}

void WindowsInput::setMouseMode(core::MouseMode mode)
{
    int glfwMode;

    switch (mode) {
        case core::MouseMode::VISIBLE: glfwMode = GLFW_CURSOR_NORMAL; break;
        case core::MouseMode::INVISIBLE: glfwMode = GLFW_CURSOR_HIDDEN; break;
        case core::MouseMode::LOCKED: glfwMode = GLFW_CURSOR_DISABLED; break;
        case core::MouseMode::INVALID: glfwMode = GLFW_CURSOR_NORMAL; break;
        default: SirenAssert(false, "Invalid MouseMode encountered.");
    }

    glfwSetInputMode(m_window, GLFW_CURSOR, glfwMode);
}

glm::vec2 WindowsInput::getScrollDelta()
{
    return s_scrollDelta;
}

} // namespace siren::platform
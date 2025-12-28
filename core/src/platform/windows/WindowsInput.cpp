#include "WindowsInput.hpp"

#include "WindowsUtils.hpp"
#include "platform/GL.hpp"
#include "utilities/spch.hpp"


namespace siren::platform
{
using namespace siren::core;

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
} // namespace siren::platform

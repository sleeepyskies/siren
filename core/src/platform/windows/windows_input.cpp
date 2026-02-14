#include "windows_input.hpp"

#include "mappings.hpp"
#include "platform/gl.hpp"
#include "utilities/spch.hpp"


namespace siren::platform
{
using namespace siren::core;

WindowsInput::WindowsInput(GLFWwindow* window) : m_window(window) { }

bool WindowsInput::is_key_held(const KeyCode code)
{
    return glfwGetKey(m_window, to_glfw(code)) == GLFW_PRESS;
}

bool WindowsInput::is_mouse_key_held(const MouseCode code)
{
    return glfwGetMouseButton(m_window, to_glfw(code)) == GLFW_PRESS;
}

glm::vec2 WindowsInput::get_mouse_position()
{
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return { x, y };
}

void WindowsInput::set_mouse_position(const glm::vec2 position)
{
    glfwSetCursorPos(m_window, position.x, position.y);
}
} // namespace siren::platform

#include "Input.hpp"

#include "core/Application.hpp"
#include "core/Window.hpp"

namespace siren::core
{

glm::vec2 Input::s_previousMousePosition{ 0 };
glm::vec2 Input::s_currentMousePosition{ 0 };
MouseMode Input::s_mouseMode{ MouseMode::VISIBLE };
glm::vec2 Input::s_scrollDelta{ 0 };

bool Input::isKeyPressed(const KeyCode code)
{
    GLFWwindow* window = Application::get().getWindow().handle();
    return glfwGetKey(window, static_cast<int>(code)) == GLFW_PRESS;
}

bool Input::isMouseKeyPressed(const MouseCode code)
{
    GLFWwindow* window = Application::get().getWindow().handle();
    return glfwGetMouseButton(window, static_cast<int>(code)) == GLFW_PRESS;
}

glm::vec2 Input::getMousePosition()
{
    return s_currentMousePosition - s_previousMousePosition;
}

void Input::setMousePosition(const glm::vec2 position)
{
    GLFWwindow* window = Application::get().getWindow().handle();
    glfwSetCursorPos(window, position.x, position.y);
}

glm::vec2 Input::getDeltaMousePosition()
{
    return s_currentMousePosition - s_previousMousePosition;
}

MouseMode Input::getMouseMode()
{
    GLFWwindow* window = Application::get().getWindow().handle();

    const int glfwMode = glfwGetInputMode(window, GLFW_CURSOR);

    switch (glfwMode) {
        case GLFW_CURSOR_NORMAL  : return MouseMode::VISIBLE;
        case GLFW_CURSOR_HIDDEN  : return MouseMode::INVISIBLE;
        case GLFW_CURSOR_DISABLED: return MouseMode::LOCKED;
        default                  : return MouseMode::INVALID;
    }
}

void Input::setMouseMode(const MouseMode mode)
{
    s_mouseMode        = mode;
    GLFWwindow* window = Application::get().getWindow().handle();

    int glfwMode;

    switch (mode) {
        case MouseMode::VISIBLE  : glfwMode = GLFW_CURSOR_NORMAL; break;
        case MouseMode::INVISIBLE: glfwMode = GLFW_CURSOR_HIDDEN; break;
        case MouseMode::LOCKED   : glfwMode = GLFW_CURSOR_DISABLED; break;
        case MouseMode::INVALID  : glfwMode = GLFW_CURSOR_NORMAL; break;
    }

    glfwSetInputMode(window, GLFW_CURSOR, glfwMode);
}

glm::vec2 Input::getScrollDelta()
{
    return s_scrollDelta;
}

void Input::update()
{
    const Window& window = Application::get().getWindow();
    double x, y;
    glfwGetCursorPos(window.handle(), &x, &y);
    s_previousMousePosition = s_currentMousePosition;
    s_currentMousePosition  = { x, y };
    s_scrollDelta           = { 0, 0 };
}

void Input::onScrollCallback(const glm::vec2 scrollDelta)
{
    s_scrollDelta = scrollDelta;
}

} // namespace siren::core

#include "Input.hpp"

#include "core/Application.hpp"
#include "core/Window.hpp"

namespace siren::core
{

glm::vec2 Input::s_previousMousePosition{ 0 };
glm::vec2 Input::s_currentMousePosition{ 0 };
MouseMode Input::s_mouseMode{ MouseMode::VISIBLE };

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

void Input::setMouseMode(const MouseMode mode)
{
    s_mouseMode        = mode;
    GLFWwindow* window = Application::get().getWindow().handle();

    int glfwMode;

    switch (mode) {
        case MouseMode::VISIBLE: {
            glfwMode = GLFW_CURSOR_NORMAL;
            break;
        }
        case MouseMode::INVISIBLE: {
            glfwMode = GLFW_CURSOR_HIDDEN;
            break;
        }
        case MouseMode::LOCKED: {
            glfwMode = GLFW_CURSOR_DISABLED;
            break;
        }
    }

    glfwSetInputMode(window, GLFW_CURSOR, glfwMode);
}

void Input::update()
{
    Window& window = Application::get().getWindow();
    double x, y;
    glfwGetCursorPos(window.handle(), &x, &y);
    s_previousMousePosition = s_currentMousePosition;
    s_currentMousePosition  = { x, y };

    /*
    if (s_mouseMode == MouseMode::LOCKED) {
        const glm::ivec2 halfSize = window.getSize() / 2;
        setMousePosition(halfSize);
    }
*/
}

} // namespace siren::core

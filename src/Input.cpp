#include "Input.hpp"

bool Input::isKeyPressed(const KeyCode code)
{
    const auto window = Window::handle();
    return glfwGetKey(window, code) == GLFW_PRESS;
}

bool Input::isMouseKeyPressed(const MouseCode code)
{
    const auto window = Window::handle();
    return glfwGetMouseButton(window, code) == GLFW_PRESS;
}

glm::dvec2 Input::getMousePosition()
{
    const auto window = Window::handle();
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return { x, y };
}

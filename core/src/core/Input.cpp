#include "Input.hpp"

#include "core/Application.hpp"
#include "core/Window.hpp"

namespace core
{

bool Input::isKeyPressed(const KeyCode code)
{
    GLFWwindow* window = Application::get().getWindow().handle();
    return glfwGetKey(window, code) == GLFW_PRESS;
}

bool Input::isMouseKeyPressed(const MouseCode code)
{
    GLFWwindow* window = Application::get().getWindow().handle();
    return glfwGetMouseButton(window, code) == GLFW_PRESS;
}

glm::dvec2 Input::getMousePosition()
{
    GLFWwindow* window = Application::get().getWindow().handle();
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return { x, y };
}

} // namespace core

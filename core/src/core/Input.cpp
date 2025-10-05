#include "Input.hpp"

#include "core/Application.hpp"
#include "core/Window.hpp"

namespace siren::core
{

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

glm::dvec2 Input::getMousePosition()
{
    GLFWwindow* window = Application::get().getWindow().handle();
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return { x, y };
}

} // namespace siren::core

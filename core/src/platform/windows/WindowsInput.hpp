#pragma once
#include "input/Input.hpp"

namespace siren::platform
{

class WindowsInput final : public core::Input
{
public:
    explicit WindowsInput(GLFWwindow* window);

    bool isKeyPressed(core::KeyCode code) override;
    bool isMouseKeyPressed(core::MouseCode code) override;
    glm::vec2 getMousePosition() override;
    void setMousePosition(glm::vec2 position) override;
    core::MouseMode getMouseMode() override;
    void setMouseMode(core::MouseMode mode) override;
    glm::vec2 getScrollDelta() override;

private:
    /// @brief A handle to a GLFW window to avoid requesting it repeatedly.
    GLFWwindow* m_window = nullptr;
};

} // namespace siren::platform

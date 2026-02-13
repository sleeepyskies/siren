#pragma once

#include "input/input.hpp"
#include "platform/gl.hpp"


namespace siren::platform
{
class WindowsInput final : public core::Input
{
public:
    explicit WindowsInput(GLFWwindow* window);

    bool is_key_held(core::KeyCode code) override;
    bool is_mouse_key_held(core::MouseCode code) override;
    glm::vec2 get_mouse_position() override;
    void set_mouse_position(glm::vec2 position) override;
    core::MouseMode get_mouse_mode() override;
    void set_mouse_mode(core::MouseMode mode) override;

private:
    /// @brief A handle to a GLFW window to avoid requesting it repeatedly.
    GLFWwindow* m_window = nullptr;
};
} // namespace siren::platform

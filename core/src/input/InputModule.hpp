/**
 * @file InputModule.hpp
 */
#pragma once

#include "InputCodes.hpp"
#include "core/Module.hpp"
#include "input/Input.hpp"

#include <glm/vec2.hpp>

namespace siren::core
{

/**
 * @brief InputModule handles direct accessing input status, as well as manipulating input items
 * such as the cursor. It always checks only the currently focused window.
 */
class InputModule final : public Module
{
public:
    bool initialize() override;
    void shutdown() override{};

    /// @brief Updates the InputModule. Should be called once each frame.
    void update();
    /// @brief Checks whether the given @ref KeyCode is pressed down.
    bool isKeyPressed(KeyCode code) const;
    /// @brief Checks whether the given @ref MouseCode is pressed down.
    bool isMouseKeyPressed(MouseCode code) const;
    /// @brief Returns the current position of the mouse.
    glm::vec2 getMousePosition() const;
    /// @brief Sets the mouse position.
    void setMousePosition(glm::vec2 position) const;
    /// @brief Returns the delta mouse position, aka the difference in mouse position between this
    /// and the last frame.
    glm::vec2 getDeltaMousePosition() const;
    /// @brief Returns the current @ref MouseMode.
    MouseMode getMouseMode() const;
    /// @brief Sets the current @ref MouseMode.
    void setMouseMode(MouseMode mode);
    /// @brief Gets the scroll delta, aka the difference in scroll between this frame and the last.
    /// Scroll value is always either -1, 0 or 1. Horizontal scrolling is also allowed.
    glm::vec2 getScrollDelta();

    const char* getName() override { return "InputModule"; }

private:
    Own<Input> m_input = nullptr;

    glm::vec2 m_currentMousePosition{};
    glm::vec2 m_previousMousePosition{};
    glm::vec2 m_scrollOffset{};
};

} // namespace siren::core

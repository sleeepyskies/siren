/**
 * @file InputModule.hpp
 */
#pragma once

#include "InputCodes.hpp"
#include "input/Input.hpp"

#include <glm/vec2.hpp>

#include "core/core.hpp"


namespace siren::core
{
/**
 * @brief InputModule handles direct accessing input status, as well as manipulating input items
 * such as the cursor. It always checks only the currently focused window.
 */
class InputModule final {
public:
    InputModule();

    /// @brief Updates the InputModule. Should be called once each frame.
    void update();
    /// @brief Is true only on the first frame the @ref KeyCode is pressed.
    bool is_key_pressed(KeyCode code) const;
    /// @brief Is true for every frame the @ref KeyCode is pressed.
    bool is_key_held(KeyCode code) const;
    /// @brief Is true only on the first frame the @ref KeyCode is released.
    bool is_key_released(KeyCode code) const;
    /// @brief Is true only on the first frame the @ref MouseCode is pressed.
    bool is_mouse_key_pressed(MouseCode code) const;
    /// @brief Is true for every frame the @ref MouseCode is pressed.
    bool is_mouse_key_held(MouseCode code) const;
    /// @brief Is true only on the first frame the @ref MouseCode is released.
    bool is_mouse_key_released(MouseCode code) const;
    /// @brief Returns the current position of the mouse.
    glm::vec2 mouse_position() const;
    /// @brief Returns the delta mouse position, aka the difference in mouse position between this
    /// and the last frame.
    glm::vec2 delta_mouse_position() const;
    /// @brief Gets the scroll delta, aka the difference in scroll between this frame and the last.
    /// Scroll value is always either -1, 0 or 1. Horizontal scrolling is also allowed.
    glm::vec2 scroll_delta() const;

private:
    glm::vec2 m_current_mouse_position{ };
    glm::vec2 m_previous_mouse_position{ };
    glm::vec2 m_scroll_offset{ };

    std::bitset<static_cast<size_t>(KeyCode::MAX)> m_previous_keys{ };
    std::bitset<static_cast<size_t>(KeyCode::MAX)> m_keys{ };
    std::bitset<static_cast<size_t>(MouseCode::MAX)> m_mousekeys{ };
    std::bitset<static_cast<size_t>(MouseCode::MAX)> m_previous_mousekeys{ };
};
} // namespace siren::core

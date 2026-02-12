#pragma once

#include "input_codes.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
/**
 * @brief Input defines an abstraction over the underlying OS. Each implementation of Input handles
 * OS specific queries of input.
 */
class Input {
public:
    virtual ~Input() = default;

    /// @brief Checks whether the given @ref KeyCode is pressed down.
    virtual auto is_key_held(KeyCode code) -> bool = 0;
    /// @brief Checks whether the given @ref MouseCode is pressed down.
    virtual auto is_mouse_key_held(MouseCode code) -> bool = 0;
    /// @brief Returns the current position of the mouse.
    virtual auto get_mouse_position() -> glm::vec2 = 0;
    /// @brief Sets the mouse position.
    virtual auto set_mouse_position(glm::vec2 position) -> void = 0;
    /// @brief Returns the current @ref MouseMode.
    virtual auto get_mouse_mode() -> MouseMode = 0;
    /// @brief Sets the current @ref MouseMode.
    virtual auto set_mouse_mode(MouseMode mode) -> void = 0;
};
} // namespace siren::core

/**
 * @file Input.hpp
 */
#pragma once

#include "InputCodes.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

/**
 * @brief Input defines an abstraction over the underlying OS. Each implementation of Input handles
 * OS specific queries of input.
 */
class Input
{
public:
    virtual ~Input() = default;

    /// @brief Checks whether the given @ref KeyCode is pressed down.
    virtual bool isKeyPressed(KeyCode code) = 0;
    /// @brief Checks whether the given @ref MouseCode is pressed down.
    virtual bool isMouseKeyPressed(MouseCode code) = 0;
    /// @brief Returns the current position of the mouse.
    virtual glm::vec2 getMousePosition() = 0;
    /// @brief Sets the mouse position.
    virtual void setMousePosition(glm::vec2 position) = 0;
    /// @brief Returns the current @ref MouseMode.
    virtual MouseMode getMouseMode() = 0;
    /// @brief Sets the current @ref MouseMode.
    virtual void setMouseMode(MouseMode mode) = 0;
};

} // namespace siren::core

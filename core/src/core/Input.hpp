#pragma once

#include "InputCodes.hpp"
#include <glm/vec2.hpp>

namespace siren::core
{

class Input
{
public:
    static bool isKeyPressed(KeyCode code);
    static bool isMouseKeyPressed(MouseCode code);

    static glm::vec2 getMousePosition();
    static void setMousePosition(glm::vec2 position);
    static glm::vec2 getDeltaMousePosition();

    static MouseMode getMouseMode();
    static void setMouseMode(MouseMode mode);

    /// @brief Should be called once per frame before polling events
    static void update();

private:
    static glm::vec2 s_previousMousePosition;
    static glm::vec2 s_currentMousePosition;
    static MouseMode s_mouseMode;
};

} // namespace siren::core

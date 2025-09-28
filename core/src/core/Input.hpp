#pragma once

#include "InputCodes.hpp"
#include <glm/vec2.hpp>

namespace core
{

class Input
{
public:
    static bool isKeyPressed(KeyCode code);
    static bool isMouseKeyPressed(MouseCode code);
    static glm::dvec2 getMousePosition();
};

} // namespace core

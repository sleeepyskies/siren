#pragma once

#include "utilities/spch.hpp"

namespace core
{

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 uv;
};

} // namespace core

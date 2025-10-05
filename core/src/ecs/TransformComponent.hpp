#pragma once

#include "ecs/Component.hpp"
#include "utilities/spch.hpp"

namespace siren::ecs
{
struct TransformComponent final : secs::Component {
    glm::mat4 transform{ 1 };
};

} // namespace siren::ecs

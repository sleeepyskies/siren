#pragma once

#include "ecs/core/Component.hpp"
#include <glm/gtc/quaternion.hpp>

namespace siren::ecs
{
struct TransformComponent final : ecs::Component {
    glm::vec3 position{ 0.0f };
    glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 scale{ 1.0f };

    glm::mat4 getTransform() const
    {
        return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) *
               glm::scale(glm::mat4(1.0f), scale);
    }
};

} // namespace siren::ecs

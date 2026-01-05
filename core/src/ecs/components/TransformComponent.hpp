#pragma once

#include "ecs/core/Component.hpp"
#include <glm/gtc/quaternion.hpp>


namespace siren::core
{
// todo: no quaternion here please

struct TransformComponent final : Component
{
    TransformComponent() = default;
    TransformComponent(const glm::vec3& t, const glm::vec3& r, const glm::vec3 s) : translation(t), rotation(r),
        scale(s) { }

    glm::vec3 translation{ 0.f };
    glm::vec3 rotation{ 0.f };
    glm::vec3 scale{ 1.f };

    glm::mat4 GetTransform() const
    {
        const glm::mat4 rotMat =
                glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0)) *
                glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0)) *
                glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));

        return glm::translate(glm::mat4(1.0f), translation) * rotMat *
                glm::scale(glm::mat4(1.0f), scale);
    }
};
} // namespace siren::ecs

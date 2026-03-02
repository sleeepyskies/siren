#pragma once

#include "ecs/core/Component.hpp"


namespace siren::core
{
struct PointLightComponent final : Component
{
    glm::vec3 color{ 1 };

    explicit PointLightComponent(const glm::vec3& color)
        : color(color) { }
    PointLightComponent() = default;
};
} // namespace siren::ecs

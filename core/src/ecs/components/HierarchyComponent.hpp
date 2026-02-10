#pragma once
#include <ecs/core/Component.hpp>
#include <ecs/core/EntityManager.hpp>
#include <utilities/UUID.hpp>


namespace siren::core
{

struct HierarchyComponent final : Component {
    EntityHandle parent = EntityHandle::invalid();
    std::vector<EntityHandle> children{ };

    HierarchyComponent() = default;

    explicit HierarchyComponent(const EntityHandle parent) : parent(parent) { }
};

} // namespace siren::ecs

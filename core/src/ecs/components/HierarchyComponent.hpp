#pragma once
#include <ecs/core/Component.hpp>
#include <ecs/core/EntityManager.hpp>
#include <utilities/UUID.hpp>

namespace siren::ecs
{

struct HierarchyComponent final : Component {
    EntityHandle parent = utilities::UUID::invalid();
    std::vector<EntityHandle> children{};
};

} // namespace siren::ecs
#include "EntityManager.hpp"

namespace siren::ecs
{

EntityHandle EntityManager::createEntity()
{
    const EntityHandle e{};
    m_entityToMask[e] = ComponentMask{};
    return e;
}

bool EntityManager::destroyEntity(EntityHandle entity)
{
    if (!entity) { return false; }
    m_entityToMask.erase(entity);
    entity.invalidate();
    return true;
}

std::vector<EntityHandle> EntityManager::getEntitiesWith(ComponentMask components)
{
    std::vector<EntityHandle> entities{};
    for (const auto& [handle, mask] : m_entityToMask) {
        if ((mask & components) == components) { entities.push_back(handle); }
    }
    return entities;
}

} // namespace siren::ecs

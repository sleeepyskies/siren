#include "EntityManager.hpp"

namespace siren::ecs
{

EntityHandle EntityManager::create()
{
    const EntityHandle e{};
    m_entityToMask[e] = ComponentMask{};
    return e;
}

void EntityManager::destroy(EntityHandle entity)
{
    if (!entity) { return; }
    m_entityToMask.erase(entity);
    entity.invalidate();
}

std::vector<EntityHandle> EntityManager::getWith(ComponentMask components)
{
    // This is probably the best solution with the current setup, but might have to rework whole ecs
    // if things start slowing down
    std::vector<EntityHandle> entities{};
    for (const auto& [handle, mask] : m_entityToMask) {
        if ((mask & components) == components) { entities.push_back(handle); }
    }
    return entities;
}

} // namespace siren::ecs

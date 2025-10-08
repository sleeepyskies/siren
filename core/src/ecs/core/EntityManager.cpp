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

} // namespace siren::ecs
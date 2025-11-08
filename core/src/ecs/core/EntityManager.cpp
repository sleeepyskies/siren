#include "EntityManager.hpp"


namespace siren::core
{

EntityHandle EntityManager::create()
{
    const EntityHandle e = EntityHandle::create();

    SirenAssert(e, "Created invalid entity (idk how).");
    SirenAssert(!m_entityToMask.contains(e), "Created already existing entity");
    SirenAssert(!m_entityToIndex.contains(e), "Created already existing entity");

    m_entityToMask[e] = ComponentMask{ };
    m_alive.push_back(e);
    m_entityToIndex[e] = m_alive.size() - 1;

    return e;
}

void EntityManager::destroy(EntityHandle entity)
{
    if (!entity) { return; }

    m_alive[m_entityToIndex[entity]]                  = m_alive.back();
    m_entityToIndex[m_alive[m_entityToIndex[entity]]] = m_entityToIndex[entity];

    m_alive.pop_back();

    m_entityToIndex.erase(entity);
    m_entityToMask.erase(entity);
    entity.invalidate();
}

Vector<EntityHandle> EntityManager::getWith(const ComponentMask components) const
{
    // This is probably the best solution with the current setup, but might have to rework whole ecs
    // if things start slowing down
    Vector<EntityHandle> entities{ };
    for (const auto& [handle, mask] : m_entityToMask) {
        if ((mask & components) == components) { entities.push_back(handle); }
    }
    return entities;
}

Vector<EntityHandle> EntityManager::getAll() const
{
    return m_alive;
}

} // namespace siren::ecs

#include "World.hpp"

#include <ecs/components/HierarchyComponent.hpp>


namespace siren::core
{
EntityHandle World::Create()
{
    const auto entity = m_entityManager.create();
    trc("Created new entity {}", entity);
    return entity;
}

void World::destroy(const EntityHandle entity)
{
    if (!entity) {
        dbg("Cannot destroy invalid entity");
        return;
    }
    trc("Destroyed entity {}", entity);
    m_entityManager.destroy(entity);
    m_componentManager.destroy(entity);
}

void World::onUpdate(const float delta)
{
    if (m_isPaused) { return; }
    m_systemManager.onUpdate(delta, *this);
}

void World::onRender()
{
    m_systemManager.onRender(*this);
}

void World::pause()
{
    if (!m_isPaused) {
        m_systemManager.onPause(*this);
        m_isPaused = true;
        trc("Scene Paused");
    }
}

void World::resume()
{
    if (m_isPaused) {
        m_systemManager.onResume(*this);
        m_isPaused = false;
        trc("Scene Resumed");
    }
}

} // namespace siren::ecs

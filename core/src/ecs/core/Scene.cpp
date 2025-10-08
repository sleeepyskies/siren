#include "Scene.hpp"

namespace siren::ecs
{
Entity Scene::createEntity()
{
    const Entity e = m_entityManager.createEntity();
    SirenAssert(e, "Created invalid entity.");
    return e;
}

bool Scene::destroyEntity(const Entity entity)
{
    if (!entity) {
        wrn("Attempting to delete a non-existing entity");
        return false;
    }
    return m_entityManager.destroyEntity(entity) && m_componentManager.entityDestroyed(entity);
}

void Scene::onUpdate(const float delta)
{
    m_systemManager.onUpdate(delta, *this);
}

} // namespace siren::ecs
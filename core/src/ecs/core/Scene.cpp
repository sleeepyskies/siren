#include "Scene.hpp"

namespace siren::ecs
{
EntityHandle Scene::createEntity()
{
    const EntityHandle e = m_entityManager.createEntity();
    SirenAssert(e, "Created invalid entity."); // should not happen lol
    return e;
}

bool Scene::destroyEntity(const EntityHandle entity)
{
    if (!entity) {
        wrn("Attempting to delete a non-existing entity");
        return false;
    }
    return m_entityManager.destroyEntity(entity) && m_componentManager.onEntityDestroyed(entity);
}

void Scene::onUpdate(const float delta)
{
    m_systemManager.onUpdate(delta, *this);
}

} // namespace siren::ecs

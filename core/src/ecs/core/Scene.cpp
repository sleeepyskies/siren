#include "Scene.hpp"

namespace siren::ecs
{
EntityHandle Scene::create()
{
    return m_entityManager.create();
}

void Scene::destroy(EntityHandle entity)
{
    if (!entity) {
        dbg("Cannot destroy invalid entity");
        return;
    }
    m_entityManager.destroy(entity);
    m_componentManager.destroy(entity);
}

void Scene::onUpdate(const float delta)
{
    m_systemManager.onUpdate(delta, *this);
}

} // namespace siren::ecs

#include "Scene.hpp"

#include <ecs/components/HierarchyComponent.hpp>

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
    if (m_isPaused) { return; }
    m_systemManager.onUpdate(delta, *this);
}

void Scene::onRender()
{
    m_systemManager.onRender(*this);
}

void Scene::addChild(const EntityHandle parent, const EntityHandle child)
{
    auto& parentHierarchy = emplace<HierarchyComponent>(parent);
    auto& childHierarchy  = emplace<HierarchyComponent>(child);

    // TODO: maybe we want to overwrite? maybe some other functions explicitly for that?
    SirenAssert(!childHierarchy.parent, "Cannot overwrite a child's parent");

    parentHierarchy.children.push_back(child);
    childHierarchy.parent = parent;
}

void Scene::pause()
{
    if (!m_isPaused) {
        m_systemManager.onPause(*this);
        m_isPaused = true;
        dbg("Scene Paused");
    }
}

void Scene::resume()
{
    if (m_isPaused) {
        m_systemManager.onResume(*this);
        m_isPaused = false;
        dbg("Scene Resumed");
    }
}

} // namespace siren::ecs

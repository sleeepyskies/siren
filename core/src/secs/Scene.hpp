#pragma once

#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "secsAssert.hpp"

namespace core::secs
{
/**
 * @brief The Scene class acts as an API for the Entity-Component-System in sle. It manages the
 * lifetime of all ECS related objects, and allows for creation, deletion and updating of these
 * objects.
 */
class Scene
{
public:
    Scene()
        : m_entityManager(EntityManager()), m_componentManager(ComponentManager()),
          m_systemManager(SystemManager())
    {
    }

    ~Scene() = default;

    /// @brief Create and return a brand-new @ref Entity.
    Entity createEntity()
    {
        const Entity e = m_entityManager.createEntity();
        SECS_ASSERT(e, "Created invalid entity.");
        return e;
    }

    /// @brief Destroys the given entity.
    void destroyEntity(Entity& entity)
    {
        SECS_ASSERT(entity, "Cannot destroy an entity that is not alive.");
        m_entityManager.destroyEntity(entity);
        m_componentManager.entityDestroyed(entity);
        entity.kill();
    }

    /// @brief Creates an internal relation between the Entity and the Component in the system.
    /// Note that secs will create a copy of the component passed to store internally, such that
    /// the caller cannot make manual changes to the components passed tos secs.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    void registerComponent(Entity& entity, const T& component)
    {
        SECS_ASSERT(entity, "Cannot register component for entity that is not alive.");
        m_entityManager.assignComponent<T>(entity);
        m_componentManager.registerComponent<T>(entity, component);
    }

    /// @brief Deletes the relation between the entity and the component of type T.
    template <typename T> void unregisterComponent(Entity& entity)
    {
        SECS_ASSERT(entity, "Cannot unregister component for entity that is not alive.");
        m_entityManager.removeComponent<T>(entity);
        m_componentManager.unregisterComponent<T>(entity);
    }

    /// @brief Gets the component of type T associated with this entity.
    template <typename T> T& getComponent(const EntityID eid)
    {
        SECS_ASSERT(eid != 0, "Cannot get component for entity that is not alive.");
        return m_componentManager.getComponent<T>(eid);
    }

    /// @brief Returns all active components of this type.
    template <typename T> ComponentList<T>& getList()
    {
        return m_componentManager.getList<T>();
    }

    /// @brief Returns all entities that have the given components.
    template <typename... Args> std::vector<EntityID> getComponentEntities()
    {
        std::vector<EntityID> entities{};
        for (const EntityID& e : m_entityManager.entities()) {
            if ((m_componentManager.hasComponent<Args>(e) && ...)) { entities.push_back(e); }
        }
        return entities;
    }

    /// @brief Registers a system to be active.
    template <typename T> ref<System> registerSystem()
    {
        return m_systemManager.registerSystem<T>();
    }

    /// @brief Unregisters a system.
    template <typename T> void unregisterSystem()
    {
        m_systemManager.unregisterSystem<T>();
    }

    /// @brief Calls the update method of all active systems.
    void onUpdate(const float delta)
    {
        m_systemManager.onUpdate(delta, *this);
    }

    void setName(const std::string& name)
    {
        m_name = name;
    }
    std::string name()
    {
        return m_name;
    }

private:
    EntityManager m_entityManager;
    ComponentManager m_componentManager;
    SystemManager m_systemManager;

    std::string m_name = "unnamed";
};
} // namespace core::secs

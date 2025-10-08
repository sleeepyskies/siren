#pragma once

#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "ecs/core/ComponentBitMap.hpp"
#include "ecs/core/EntityManager.hpp"
#include "utilities/spch.hpp"

namespace siren::ecs
{

/**
 * @brief The Scene class acts as an API for the Entity-Component-System in sle. It manages the
 * lifetime of all ECS related objects, and allows for creation, deletion and updating of these
 * objects.
 */
class Scene
{
public:
    Scene()  = default;
    ~Scene() = default;

    /// @brief Create and return an EntityHandle
    EntityHandle createEntity();

    /// @brief Destroys the given entity.
    bool destroyEntity(EntityHandle entity);

    /// @brief Creates an internal relation between the Entity and the Component in the scene.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    bool registerComponent(const EntityHandle entity, const T& component)
    {
        if (!entity) {
            wrn("Attempting to register a component to a non existing entity");
            return false;
        }

        if (!m_entityManager.addComponent<T>(entity)) {
            m_entityManager.removeComponent<T>(entity);
            wrn("Could not register component to entity");
            return false;
        }

        return m_componentManager.registerComponent(entity, component);
    }

    /// @brief Deletes the relation between the entity and the component of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    bool unregisterComponent(EntityHandle entity)
    {
        if (!entity) {
            wrn("Attempting to unregister a component from a non existing entity");
            return false;
        }

        return m_entityManager.removeComponent<T>(entity) &&
               m_componentManager.unregisterComponent<T>(entity);
    }

    /// @brief Gets the component of type T associated with this entity.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    Maybe<T&> getComponent(const EntityHandle entity)
    {
        if (!entity) {
            wrn("Attempting to unregister a component from a non existing entity");
            return Nothing;
        }
        return m_componentManager.getComponent<T>(entity);
    }

    /// @brief Returns all entities that have the given components.
    template <typename... Args>
    std::vector<EntityHandle> getEntitiesWith()
    {
        ComponentMask requiredComponents{};
        // fold expression, applies the LHS expression to each T in Args
        (requiredComponents.set(ComponentBitMap::getBitIndex<Args>()), ...);

        return m_entityManager.getEntitiesWith(requiredComponents);
    }

    /// @brief Registers a system to be active.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool registerSystem()
    {
        return m_systemManager.registerSystem<T>(*this);
    }

    /// @brief Unregisters a system.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool unregisterSystem()
    {
        return m_systemManager.unregisterSystem<T>(*this);
    }

    /// @brief Calls the update method of all active systems.
    void onUpdate(float delta);

private:
    EntityManager m_entityManager{};
    ComponentManager m_componentManager{};
    SystemManager m_systemManager{};
};
} // namespace siren::ecs

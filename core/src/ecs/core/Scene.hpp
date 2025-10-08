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
    EntityHandle create();

    /// @brief Destroys the given entity.
    void destroy(EntityHandle entity);

    /// @brief Default creates a component of type T and assigns it to the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    void add(const EntityHandle entity)
    {
        if (!entity) {
            dbg("Attempting to register a component to a non existing entity");
            return;
        }

        m_entityManager.add<T>(entity);
        m_componentManager.add<T>(entity);
    }

    /// @brief Deletes the relation between the entity and the component of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    void remove(EntityHandle entity)
    {
        if (!entity) {
            dbg("Attempting to unregister a component from a non existing entity");
            return;
        }

        m_entityManager.remove<T>(entity);
        m_componentManager.remove<T>(entity);
    }

    /// @brief An unsafe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T& get(const EntityHandle entity)
    {
        SirenAssert(entity, "Attempting to unregister a component from a non existing entity");
        return m_componentManager.get<T>(entity);
    }

    /// @brief A safe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T* getSafe(const EntityHandle entity)
    {
        if (!entity) { return nullptr; }
        return m_componentManager.getSafe<T>(entity);
    }

    /// @brief Returns all entities that have the given components.
    template <typename... Args>
    std::vector<EntityHandle> getWith()
    {
        ComponentMask requiredComponents{};
        // fold expression, applies the LHS expression to each T in Args
        (requiredComponents.set(ComponentBitMap::getBitIndex<Args>()), ...);

        return m_entityManager.getWith(requiredComponents);
    }

    /// @brief Registers and starts the system T. The onReady() function of T will also be called
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool start()
    {
        return m_systemManager.registerSystem<T>(*this);
    }

    /// @brief Unregisters and stops the system T. The onShutDown() function of T will also be
    /// called.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool stop()
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

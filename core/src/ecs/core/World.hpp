#pragma once

#include "ComponentManager.hpp"
#include "SingletonManager.hpp"
#include "SystemManager.hpp"
#include "entt.hpp"

#include "ecs/core/ComponentBitMap.hpp"
#include "ecs/core/EntityManager.hpp"


namespace siren::core
{
class NativeScript;
}


namespace siren::core
{
/**
 * @brief The Scene class acts as an API for the Entity-Component-System. It manages the
 * lifetime of all ECS related objects, and allows for creation, deletion and updating of these
 * objects.
 */
class World {
public:
    World()  = default;
    ~World() = default;

    /// @brief Create and return an EntityHandle
    EntityHandle Create();

    /// @brief Destroys the given entity.
    void destroy(EntityHandle entity);

    /// @brief Returns all alive entities
    std::vector<EntityHandle> getAll() const {
        return m_entityManager.getAll();
    }

    /// @brief Default creates a component of type T and assigns it to the given entity. If the
    /// component already exists on this entity, nothing is changed and a reference to the existing
    /// component is returned.
    template <typename T, typename... Args>
        requires(std::is_base_of_v<Component, T>)
    T& Emplace(const EntityHandle entity, Args&&... args) {
        SIREN_ASSERT(entity, "Attempting to register a component to a non existing entity");

        m_entityManager.add<T>(entity);
        trc("Added {} to entity {}", entt::type_name<T>().value(), entity);
        return m_componentManager.emplace<T>(entity, std::forward<Args>(args)...);
    }

    /// @brief Deletes the relation between the entity and the component of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    void remove(EntityHandle entity) {
        if (!entity) {
            Logger::ecs->debug("Attempting to unregister a component from a non existing entity");
            return;
        }

        trc("Removed {} from entity {}", entt::type_name<T>().value(), entity);
        m_entityManager.remove<T>(entity);
        m_componentManager.remove<T>(entity);
    }

    template <typename T>
        requires(std::derived_from<T, NativeScript>)
    void bind(const EntityHandle entity);

    /// @brief Default constructs a singleton component. These are unique in the whole scene
    template <typename T, typename... Args>
        requires(std::is_base_of_v<Component, T>)
    T& emplaceSingleton(Args&&... args) {
        trc("Adding singleton {}", entt::type_name<T>().value());
        return m_singletonManager.emplaceSingleton<T>(std::forward<Args>(args)...);
    }

    /// @brief Removes the singleton component T if it is present, otherwise nothing happens
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    void removeSingleton() {
        trc("Removing singleton {}", entt::type_name<T>().value());
        m_singletonManager.removeSingleton<T>();
    }

    /// @brief Returns a reference to the singleton of type T. Requires that the singleton exists so
    /// make sure it does!
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T& GetSingleton() const {
        return static_cast<T&>(m_singletonManager.getSingleton<T>());
    }

    /// @brief Returns a raw pointer to the singleton of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T* GetSingletonSafe() const {
        return m_singletonManager.getSingletonSafe<T>();
    }

    /// @brief An unsafe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T& get(const EntityHandle entity) {
        SIREN_ASSERT(entity, "Performing unsafe get on a non existing entity.");
        return m_componentManager.get<T>(entity);
    }

    /// @brief An unsafe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T& get(const EntityHandle entity) const {
        SIREN_ASSERT(entity, "Performing unsafe get on a non existing entity.");
        return m_componentManager.get<T>(entity);
    }

    /// @brief A safe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T* GetSafe(const EntityHandle entity) const {
        if (!entity) { return nullptr; }
        return m_componentManager.get_safe<T>(entity);
    }

    /// @brief Returns all entities that have the given components.
    template <typename... Args>
    std::vector<EntityHandle> GetWith() const {
        EntityManager::ComponentMask requiredComponents{ };
        // fold expression, applies the LHS expression to each T in Args
        (requiredComponents.set(ComponentBitMap::getBitIndex<Args>()), ...);

        return m_entityManager.getWith(requiredComponents);
    }

    /// @brief Registers and starts the system T. The onReady() function of T will also be called
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool start(const SystemPhase phase) {
        return m_systemManager.registerSystem<T>(*this, phase);
    }

    /// @brief Unregisters and stops the system T. The onShutDown() function of T will also be
    /// called.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool stop() {
        return m_systemManager.unregisterSystem<T>(*this);
    }

    /// @brief Checks if the given entity has a component of type T.
    template <typename T>
    bool hasComponent(const EntityHandle entity) const {
        return m_componentManager.hasComponent<T>(entity);
    }

    /// @brief Calls the onUpdate method of all active systems.
    void onUpdate(float delta);

    /// @brief Calls the onDraw method of all active systems.
    void onRender();

    /// @brief Pauses the scene from updating. This means any update calls will have no effect.
    void pause();
    /// @brief Resumes the scene.
    void resume();

private:
    EntityManager m_entityManager{ };
    ComponentManager m_componentManager{ };
    SystemManager m_systemManager{ };
    SingletonManager m_singletonManager{ };

    bool m_isPaused{ false };
};
} // namespace siren::ecs

#pragma once

#include "ComponentList.hpp"
#include "EntityManager.hpp"
#include "utilities/spch.hpp"

namespace siren::ecs
{

/**
 * @brief The ComponentManager is responsible for managing which exact Components belong to which
 * Entities. Furthermore, it provides lists of each component type.
 */
class ComponentManager
{
public:
    /// @brief Registers the given component to the given entity.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    bool registerComponent(const EntityHandle entity, const T& component)
    {
        if (hasComponent<T>(entity)) { return false; }

        ComponentList<T>& list = getCreateComponentList<T>();
        list.insert(component); // wont insert a dupe

        m_entityToComponent[entity][index<T>()] = component.handle;
        m_componentToEntity[component.handle].insert(entity);

        return true;
    }

    /// @brief Unregisters the given component from the given entity.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    bool unregisterComponent(const EntityHandle entity)
    {
        if (!hasComponent<T>(entity)) { return false; }

        const Maybe<T&> component = getComponent<T>(entity);
        if (!component) { return false; }

        m_componentToEntity[component.handle].erase(entity);
        m_entityToComponent[entity][index<T>()] = INVALID_COMPONENT;

        // no entity is useing this component anymore, delete it from memory
        if (m_componentToEntity[component.handle].empty()) {
            ComponentList<T>& list = getCreateComponentList<T>();
            list.remove(component);
        }
    }

    /// @brief Should be called each time an entity is destroyed. Removes all state stored about
    /// this entity.
    bool onEntityDestroyed(const EntityHandle entity)
    {
        if (!m_entityToComponent.contains(entity)) { return false; }

        for (const auto componentHandle : m_entityToComponent[entity]) {
            m_componentToEntity[componentHandle].erase(entity);
        }

        m_entityToComponent.erase(entity);

        return true;
    }

    /// @brief Checks if the entity has this component type.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    bool hasComponent(const EntityHandle entity)
    {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        return m_entityToComponent[entity][componentIndex] != INVALID_COMPONENT;
    }

    /// @brief Returns a list reference of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    ComponentList<T>& getCreateComponentList()
    {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        if (!m_components[componentIndex]) {
            m_components[componentIndex] = makeRef<ComponentList<T>>();
        }
        return static_cast<ComponentList<T>&>(*m_components[componentIndex]);
    }

    /// @brief Gets the component of type T associated with the given entity.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    Maybe<T&> getComponent(const EntityHandle entity)
    {
        if (!hasComponent<T>(entity)) { return Nothing; }

        ComponentList<T>& list       = getCreateComponentList<T>();
        const ComponentHandle handle = m_entityToComponent[entity][index<T>()];
        return list.getComponentByHandle(handle);
    }

private:
    /// @brief All of the component lists
    std::array<Ref<IComponentList>, MAX_COMPONENTS> m_components{};
    /// @brief Mapping of EntityHandle to its assigned componentID's. Indexing into the vector is
    /// done by taking the component types index via the ComponentBitMap.
    HashMap<EntityHandle, std::array<ComponentHandle, MAX_COMPONENTS>> m_entityToComponent{};
    /// @brief Map of ComponentID to all Entities who have this component
    HashMap<ComponentHandle, HashSet<EntityHandle>> m_componentToEntity{};
};
} // namespace siren::ecs

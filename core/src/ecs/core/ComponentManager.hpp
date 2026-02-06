#pragma once

#include "ComponentList.hpp"
#include "Component.hpp"
#include "EntityManager.hpp"
#include "core/Core.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
/**
 * @brief The ComponentManager is responsible for managing which exact Components belong to which
 * Entities. Furthermore, it provides lists of each component type.
 */
class ComponentManager {
public:
    /// @brief Create Component of type T and assign it to the provided entity. If the entity
    /// already has a component of this type, do nothing.
    template <typename T, typename... Args>
        requires(std::is_base_of_v<Component, T>)
    T& emplace(const EntityHandle entity, Args&&... args) {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        if (hasComponent<T>(entity)) {
            ComponentList<T>& list       = getCreateComponentList<T>();
            const ComponentHandle handle = m_entity_to_component[entity][componentIndex];
            return list.get(handle);
        }

        ComponentList<T>& list = getCreateComponentList<T>();

        T& component                                       = list.emplace(std::forward<Args>(args)...);
        m_component_to_index[component.component_handle()] = componentIndex;
        m_entity_to_component[entity][componentIndex]      = component.component_handle();

        return component;
    }

    /// @brief Removes the Component of type T from entity. If entity does not have a component of
    /// type T, do nothing.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    void remove(const EntityHandle entity) {
        if (!hasComponent<T>(entity)) { return; }

        const size_t componentIndex     = ComponentBitMap::getBitIndex<T>();
        ComponentHandle componentHandle = m_entity_to_component[entity][componentIndex];
        ComponentList<T>& list          = getCreateComponentList<T>();

        m_component_to_index.erase(componentHandle);
        m_entity_to_component[entity][componentIndex] = INVALID_COMPONENT;
        list.remove(componentHandle);
    }

    /// @brief Should be called each time an entity is destroyed. Removes all state stored about
    /// this entity.
    void destroy(const EntityHandle entity) {
        if (!m_entity_to_component.contains(entity)) { return; }

        for (const auto& componentHandle : m_entity_to_component[entity]) {
            if (componentHandle == INVALID_COMPONENT) { continue; }
            const size_t index = m_component_to_index[componentHandle];
            m_components[index]->remove(componentHandle);
            m_component_to_index.erase(componentHandle);
        }

        m_entity_to_component.erase(entity);
    }

    /// @brief An unsafe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T& get(const EntityHandle entity) {
        const size_t componentIndex  = ComponentBitMap::getBitIndex<T>();
        const ComponentHandle handle = m_entity_to_component.at(entity)[componentIndex];
        ComponentList<T>& list       = getCreateComponentList<T>();
        return list.get(handle);
    }

    /// @brief An unsafe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T& get(const EntityHandle entity) const {
        const size_t componentIndex  = ComponentBitMap::getBitIndex<T>();
        const ComponentHandle handle = m_entity_to_component.at(entity)[componentIndex];
        ComponentList<T>& list       = getCreateComponentList<T>();
        return list.get(handle);
    }

    /// @brief A safe get of the component of type T associated with the given entity
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T* get_safe(const EntityHandle entity) const {
        const size_t componentIndex  = ComponentBitMap::getBitIndex<T>();
        const ComponentHandle handle = m_entity_to_component.at(entity)[componentIndex];
        ComponentList<T>& list       = getCreateComponentList<T>();
        return list.get_safe(handle);
    }

    /// @brief Checks if the entity has this component type.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    bool hasComponent(const EntityHandle entity) const {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        if (!m_entity_to_component.contains(entity)) { return false; }
        return m_entity_to_component.at(entity)[componentIndex] != INVALID_COMPONENT;
    }

private:
    /// @brief All the component lists
    mutable std::array<std::unique_ptr<IComponentList>, MAX_COMPONENTS> m_components{ };
    /// @brief Mapping of EntityHandle to its assigned componentID's. Indexing into the vector is
    /// done by taking the component types index via the ComponentBitMap.
    std::unordered_map<EntityHandle, std::array<ComponentHandle, MAX_COMPONENTS>> m_entity_to_component{ };
    // HACK: this is a terrible solution, but cant think of anything better for now
    /// @brief A mapping of each ComponentHandle to it index into m_components
    std::unordered_map<ComponentHandle, size_t> m_component_to_index{ };

    /// @brief Returns a list reference of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    ComponentList<T>& getCreateComponentList() const {
        const size_t component_index = ComponentBitMap::getBitIndex<T>();
        if (!m_components[component_index]) {
            m_components[component_index] = std::make_unique<ComponentList<T>>();
        }
        return static_cast<ComponentList<T>&>(*m_components[component_index]);
    }
};
} // namespace siren::ecs

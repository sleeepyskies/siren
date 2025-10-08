#pragma once

#include "Component.hpp"

namespace siren::ecs
{

/**
 * @brief Used to enable polymorphism.
 */
class IComponentList
{
public:
    virtual ~IComponentList() = default;
};

/**
 * @brief Represents a list of a single component type.
 */
template <typename T>
    requires(std::is_base_of_v<Component, T>)
class ComponentList final : public IComponentList
{
public:
    /// @brief Inserts a Component into the array.
    bool insert(const T& component)
    {
        if (m_componentToIndex.contains(component.handle)) { return false; }

        m_list.push_back(component);
        const size_t index                   = m_list.size() - 1;
        m_componentToIndex[component.handle] = index;

        return true;
    }

    /// @brief Removes a component from the list
    bool remove(const T& component)
    {
        if (!m_componentToIndex.contains(component.handle)) { return false; }

        // swap with last and pop back
        const size_t index = m_componentToIndex[component.handle];
        m_componentToIndex.erase(component.handle);
        m_list[index] = m_list[m_list.size() - 1];
        m_list.pop_back();

        return true;
    }

    /// @brief Returns the component instance with the given handle.
    Maybe<T&> getComponentByHandle(const ComponentHandle handle)
    {
        if (!m_componentToIndex.contains(handle)) { return Nothing; }
        return m_list[m_componentToIndex[handle]];
    }

    /// @brief Checks whether this Component exists in the list or not.
    bool contains(const ComponentHandle handle) const
    {
        return m_componentToIndex.contains(handle);
    }

    /// @brief Returns an iterator begin over the component list.
    std::vector<T>::iterator begin()
    {
        return m_list.begin();
    }

    /// @brief Returns an iterator end over the component list.
    std::vector<T>::iterator end()
    {
        return m_list.end();
    }

private:
    /// @brief The dense list of Components.
    std::vector<T> m_list{};
    /// @brief A mapping of @ref ComponentHandle to its index in the list. Can also be used to test
    /// if the IComponent exists in the list.
    HashMap<ComponentHandle, size_t> m_componentToIndex{};
};

} // namespace siren::ecs

#pragma once

#include "Component.hpp"
#include "secsAssert.hpp"
#include <vector>

namespace core::secs
{

/**
 * @brief The use of this abstract non-templated class is to allow us to have multiple ComponentList
 * objects storing different underlying T's.
 */
class IComponentList
{
public:
    virtual ~IComponentList() = default;
};

/**
 * @brief Used to represent a contiguous block of components of the same type.
 */
template <typename T> class ComponentList : public IComponentList
{
public:
    /// @brief Inserts a Component into the array.
    void insert(const T& component)
    {
        SECS_ASSERT(m_list.size() < MAX_ENTITIES, "Cannot have anymore Components of this type.");
        SECS_ASSERT(!m_componentToIndex.contains(component.id),
                    "This component is already in this list.");

        m_list.push_back(component);
        const size_t index               = m_list.size() - 1;
        m_componentToIndex[component.id] = index;
    }

    /// @brief Allows deletion via ComponentID.
    void remove(const ComponentID id)
    {
        SECS_ASSERT(m_componentToIndex.contains(id),
                    "Cannot delete a component that is not the the array.");

        removeIndex(m_componentToIndex[id]);
    }

    /// @brief Returns the component instance with the given id.
    T& getComponent(const ComponentID id)
    {
        SECS_ASSERT(m_componentToIndex.contains(id),
                    "Cannot retrieve component that is not in the array.");
        return m_list[m_componentToIndex[id]];
    }

    /// @brief Checks whether this Component exists in the list or not.
    bool contains(const ComponentID id) const
    {
        return m_componentToIndex.contains(id);
    }

    /// @brief Returns an iterator begin over the dense list.
    std::vector<T>::iterator begin()
    {
        return m_list.begin();
    }
    /// @brief Returns an iterator end over the dense list.
    std::vector<T>::iterator end()
    {
        return m_list.end();
    }

private:
    /// @brief Allows deletion via index.
    void removeIndex(const size_t index)
    {
        SECS_ASSERT(index < m_list.size(),
                    "Cannot remove this component due to out of bounds indexing.");

        m_componentToIndex.erase(m_list[index].id);
        m_list[index] = m_list[m_list.size() - 1];
        m_list.pop_back();
    }

    /// @brief The dense block of Components.
    std::vector<T> m_list{};
    /// @brief A mapping of @ref ComponentID to its index in the list. Can also be used to test if
    /// the IComponent exists in the list.
    hashmap<ComponentID, size_t> m_componentToIndex{};
};

} // namespace core::secs

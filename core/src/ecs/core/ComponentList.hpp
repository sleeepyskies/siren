#pragma once

#include "Component.hpp"


namespace siren::core
{
/**
 * @brief Used to enable polymorphism.
 */
class IComponentList {
public:
    virtual ~IComponentList() = default;

    virtual void remove(ComponentHandle handle) = 0;
};

/**
 * @brief Represents a list of a single component type.
 */
template <typename T>
    requires(std::is_base_of_v<Component, T>)
class ComponentList final : public IComponentList {
public:
    /// @brief Creates a new component at the back of the list and returns it.
    template <typename... Args>
    T& emplace(Args&&... args) {
        m_list.emplace_back(args...);
        const size_t index                                   = m_list.size() - 1;
        m_componentToIndex[m_list.back().component_handle()] = index;
        return m_list.back();
    }

    /// @brief Removes a component from the list
    void remove(const ComponentHandle handle) override {
        if (!m_componentToIndex.contains(handle)) { return; }

        // swap with last and pop back
        const size_t index = m_componentToIndex[handle];
        std::swap(m_list[index], m_list.back());
        m_list.pop_back();
        m_componentToIndex[m_list[index].component_handle()] = index;
    }

    /// @brief Returns the component instance with the given handle.
    T& get(const ComponentHandle handle) {
        SIREN_ASSERT(
            m_componentToIndex.contains(handle),
            "Failed to get Component from ComponentList"
        );
        return m_list[m_componentToIndex.at(handle)];
    }

    /// @brief Returns the component instance with the given handle.
    T* GetSafe(const ComponentHandle handle) {
        if (!m_componentToIndex.contains(handle)) { return nullptr; }
        return &m_list[m_componentToIndex.at(handle)];
    }

private:
    /// @brief The dense list of Components.
    Vector<T> m_list{ };
    /// @brief A mapping of @ref ComponentHandle to its index in the list. Can also be used to test
    /// if the IComponent exists in the list.
    HashMap<ComponentHandle, size_t> m_componentToIndex{ };
};
} // namespace siren::ecs

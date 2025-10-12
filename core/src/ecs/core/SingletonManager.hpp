#pragma once

#include "ecs/core/Component.hpp"
#include "ecs/core/ComponentBitMap.hpp"
#include "utilities/spch.hpp"

namespace siren::ecs
{

/// @brief Responsible for managing the singleton components. Singleton components are globally
/// unique in the scene, and are not bound to any entity.
class SingletonManager
{
public:
    /// @brief Default constructs a singleton of type T either with default args or with the
    /// provided args
    template <typename T, typename... Args>
        requires(std::is_base_of_v<Component, T>)
    void emplaceSingleton(Args&&... args)
    {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        if (m_singletons.contains(componentIndex)) { return; }
        m_singletons.emplace(componentIndex, makeUref<T>(std::forward<Args>(args)...));
    }

    /// @brief Removes the singleton of type T if it exists
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    void removeSingleton()
    {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        if (!m_singletons.contains(componentIndex)) { return; }
        m_singletons.erase(componentIndex);
    }

    /// @brief Returns a reference to the singleton of type T. Requires that the singleton does
    /// exist, so be sure to  make sure it does!
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T& getSingleton()
    {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        SirenAssert(m_singletons.contains(componentIndex), "Cannot get non existant singleton");
        return *static_cast<T*>(*m_singletons[componentIndex]);
    }

    /// @brief Returns a raw pointer to the singleton of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    T* getSingletonSafe()
    {
        const size_t componentIndex = ComponentBitMap::getBitIndex<T>();
        if (!m_singletons.contains(componentIndex)) { return nullptr; }
        return static_cast<T*>(m_singletons[componentIndex].get());
    }

private:
    HashMap<size_t, Uref<Component>> m_singletons{};
};

} // namespace siren::ecs

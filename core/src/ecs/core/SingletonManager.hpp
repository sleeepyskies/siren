#pragma once

#include "ecs/core/Component.hpp"
#include "ecs/core/ComponentBitMap.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{

// todo: replace all getters with -> std::optional<std::reference_wrapper<T>>

/// @brief Responsible for managing the singleton components. Singleton components are globally
/// unique in the scene, and are not bound to any entity.
class SingletonManager {
public:
    /// @brief Default constructs a singleton of type T either with default args or with the
    /// provided args
    template <typename T, typename... Args>
        requires(std::is_base_of_v<Component, T>)
    auto emplace_singleton(Args&&... args) -> T& {
        const size_t componentIndex = ComponentBitMap::get_bit_index<T>();
        if (!m_singletons.contains(componentIndex)) {
            m_singletons.emplace(componentIndex, std::make_unique<T>(std::forward<Args>(args)...));
        }
        return *static_cast<T*>(m_singletons[componentIndex].get());
    }

    /// @brief Removes the singleton of type T if it exists
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    auto remove_singleton() -> void {
        const size_t component_index = ComponentBitMap::get_bit_index<T>();
        if (!m_singletons.contains(component_index)) { return; }
        m_singletons.erase(component_index);
    }

    /// @brief Returns a reference to the singleton of type T. Requires that the singleton does
    /// exist, so be sure to  make sure it does!
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    auto get_singleton() const -> T& {
        const size_t component_index = ComponentBitMap::get_bit_index<T>();
        SIREN_ASSERT(m_singletons.contains(component_index), "Cannot get non existent singleton");
        return *static_cast<T*>(m_singletons[component_index].get());
    }

    /// @brief Returns a raw pointer to the singleton of type T.
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    auto get_singleton_safe() const -> T* {
        const size_t component_index = ComponentBitMap::get_bit_index<T>();
        if (!m_singletons.contains(component_index)) { return nullptr; }
        return static_cast<T*>(m_singletons[component_index].get());
    }

private:
    mutable std::unordered_map<size_t, std::unique_ptr<Component>> m_singletons{ };
};

} // namespace siren::ecs

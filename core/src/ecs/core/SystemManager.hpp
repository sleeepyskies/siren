#pragma once

#include "System.hpp"
#include "utilities/Types.hpp"
#include "utilities/spch.hpp"

namespace siren::ecs
{

class SystemManager
{
public:
    /// @brief Registers a new system if it is not already registered and calls the onReady()
    /// method of the system.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool registerSystem(Scene& scene)
    {
        const std::type_index systemIndex = index<T>();

        if (m_systems.contains(systemIndex)) { return false; }
        auto system            = makeUref<T>();
        m_systems[systemIndex] = std::move(system);

        system->onReady(scene);

        return true;
    }

    /// @brief Unregisters the system T if it is already registered and calls the onShutDown()
    /// method of the system.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool unregisterSystem(Scene& scene)
    {
        const std::type_index systemIndex = index<T>();

        if (!m_systems.contains(systemIndex)) { return false; }

        m_systems[systemIndex]->onShutdown(scene);
        m_systems.erase(systemIndex);

        return true;
    }

    /// @brief Calls the onUpdate() method of all active systems in no specific order.
    void onUpdate(const float delta, Scene& scene) const
    {
        for (const auto& [_, system] : m_systems) { system->onUpdate(delta, scene); }
    }

    /// @brief Calls the onRender() method of all active systems in no specific order.
    void onRender(Scene& scene) const
    {
        for (const auto& [_, system] : m_systems) { system->onRender(scene); }
    }

private:
    template <typename T>
    [[nodiscard]] std::type_index index() const
    {
        return std::type_index(typeid(T));
    }

    /// @brief All the registered systems
    HashMap<std::type_index, Uref<System>> m_systems{};
};

} // namespace siren::ecs

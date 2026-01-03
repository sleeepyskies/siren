#pragma once

#include "System.hpp"
#include "SystemPhase.hpp"
#include "utilities/Types.hpp"
#include "utilities/spch.hpp"
#include <ranges>


namespace siren::core
{
class SystemManager
{
public:
    // TODO: call shutdown of all systems on destruction

    /// @brief Registers a new system if it is not already registered and calls the onReady()
    /// method of the system.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool registerSystem(Scene& scene, const SystemPhase phase)
    {
        const std::type_index systemIndex = index<T>();
        if (m_registeredSystems.contains(systemIndex)) { return false; }

        m_systems[phase][systemIndex] = createOwn<T>();
        const auto& system            = m_systems[phase][systemIndex];
        system->onReady(scene); // maybe we want to only call this on scene start

        m_registeredSystems[systemIndex] = phase;

        return true;
    }

    /// @brief Unregisters the system T if it is already registered and calls the onShutDown()
    /// method of the system.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    bool unregisterSystem(Scene& scene)
    {
        const std::type_index systemIndex = index<T>();
        if (!m_registeredSystems.contains(systemIndex)) { return false; }

        const SystemPhase phase = m_registeredSystems[systemIndex];
        m_registeredSystems.erase(systemIndex);

        m_systems[phase][systemIndex]->onShutdown(scene);
        m_systems[phase].erase(systemIndex);

        return true;
    }

    /// @brief Calls the onUpdate() method of all active systems in no specific order.
    void onUpdate(const float delta, Scene& scene) const
    {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->onUpdate(delta, scene); //
            }
        }
    }

    /// @brief Calls the onUpdate() method of all active systems in no specific order.
    void onRender(Scene& scene) const
    {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->onRender(scene); //
            }
        }
    }

    void onPause(Scene& scene) const
    {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->onPause(scene); //
            }
        }
    }

    void onResume(Scene& scene) const
    {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->onResume(scene); //
            }
        }
    }

private:
    template <typename T>
    [[nodiscard]] std::type_index index() const
    {
        return std::type_index(typeid(T));
    }

    using SystemBucket = HashMap<std::type_index, Own<System>>;

    /// @brief All the registered systems ordered by phase
    Array<SystemBucket, SystemPhaseMax> m_systems{ };

    /// @brief Unique type index per system type mapping to SystemPhase
    HashMap<std::type_index, SystemPhase> m_registeredSystems{ };
};
} // namespace siren::ecs

#pragma once

#include "System.hpp"
#include "SystemPhase.hpp"
#include "utilities/spch.hpp"
#include <ranges>


namespace siren::core
{
class SystemManager {
public:
    // TODO: call shutdown of all systems on destruction

    /// @brief Registers a new system if it is not already registered and calls the onReady()
    /// method of the system.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    auto register_system(World& scene, const SystemPhase phase) -> bool {
        const std::type_index systemIndex = index<T>();
        if (m_registered_systems.contains(systemIndex)) { return false; }

        m_systems[phase][systemIndex] = std::make_unique<T>();
        const auto& system            = m_systems[phase][systemIndex];
        system->onReady(scene); // maybe we want to only call this on scene start

        m_registered_systems[systemIndex] = phase;

        return true;
    }

    /// @brief Unregisters the system T if it is already registered and calls the onShutDown()
    /// method of the system.
    template <typename T>
        requires(std::is_base_of_v<System, T>)
    auto unregister_system(World& scene) -> bool {
        const std::type_index systemIndex = index<T>();
        if (!m_registered_systems.contains(systemIndex)) { return false; }

        const SystemPhase phase = m_registered_systems[systemIndex];
        m_registered_systems.erase(systemIndex);

        m_systems[phase][systemIndex]->onShutdown(scene);
        m_systems[phase].erase(systemIndex);

        return true;
    }

    /// @brief Calls the onUpdate() method of all active systems in no specific order.
    auto on_update(const float delta, World& scene) const -> void {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->onUpdate(delta, scene); //
            }
        }
    }

    /// @brief Calls the onUpdate() method of all active systems in no specific order.
    auto on_render(World& scene) const -> void {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->on_render(scene); //
            }
        }
    }

    auto on_pause(World& scene) const -> void {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->onPause(scene); //
            }
        }
    }

    auto on_resume(World& scene) const -> void {
        for (const auto& bucket : m_systems) {
            for (const auto& system : bucket | std::views::values) {
                system->onResume(scene); //
            }
        }
    }

private:
    template <typename T>
    [[nodiscard]] std::type_index index() const {
        return std::type_index(typeid(T));
    }

    using SystemBucket = std::unordered_map<std::type_index, std::unique_ptr<System>>;
    /// @brief All the registered systems ordered by phase
    std::array<SystemBucket, SystemPhaseMax> m_systems{ };
    /// @brief Unique type index per system type mapping to SystemPhase
    std::unordered_map<std::type_index, SystemPhase> m_registered_systems{ };
};
} // namespace siren::ecs

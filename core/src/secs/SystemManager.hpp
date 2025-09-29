#pragma once

#include "System.hpp"
#include "secsAssert.hpp"
#include "secsTypes.hpp"
#include <typeindex>

namespace core::secs
{

class SystemManager
{
public:
    SystemManager()  = default;
    ~SystemManager() = default;

    /// @brief Registers and returns a pointer to the new system. There may only be one system of a
    /// type active at a time, since systems of the same type have the same functionality anyway.
    template <typename T> ref<System> registerSystem()
    {
        SECS_ASSERT(!m_registeredSystems.contains(index<T>()),
                    "This System is already registered.");

        auto system                     = std::make_shared<T>();
        m_registeredSystems[index<T>()] = system;
        return system;
    }

    /// @brief Registers and returns a pointer to the new system. There may only be one system of a
    /// type active at a time, since systems of the same type have the same functionality anyway.
    template <typename T> void unregisterSystem()
    {
        SECS_ASSERT(m_registeredSystems.contains(index<T>()), "This System is not yet registered.");

        m_registeredSystems.erase(index<T>());
    }

    /// @brief Calls the update method of all active systems.
    void onUpdate(const float delta, Scene& scene) const
    {
        for (const auto& pair : m_registeredSystems) {
            const auto& system = pair.second;
            system->onUpdate(delta, scene);
        }
    }

private:
    template <typename T> [[nodiscard]] std::type_index index() const
    {
        return std::type_index(typeid(T));
    }

private:
    hashmap<std::type_index, ref<System>> m_registeredSystems{};
};

} // namespace core::secs

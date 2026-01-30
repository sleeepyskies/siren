#pragma once

#include "ECSProperties.hpp"
#include "utilities/spch.hpp"
#include <typeindex>

#include "core/Core.hpp"


namespace siren::core
{
struct Component;

/**
 * @brief This class handles assigning each Component Type a unique index in the range [0,
 * MAX_COMPONENTS]. This is used for setting the ComponentMask bits.
 */
class ComponentBitMap
{
public:
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    static size_t getBitIndex()
    {
        SIREN_ASSERT(
            s_nextIndex <= MAX_COMPONENTS,
            "Cannot register more components than MAX_COMPONENTS allows!"
        );

        const auto typeIndex = std::type_index(typeid(T));
        if (!s_registry.contains(typeIndex)) { s_registry[typeIndex] = s_nextIndex++; }

        return s_registry[typeIndex];
    }

private:
    static inline HashMap<std::type_index, size_t> s_registry{ };
    static inline size_t s_nextIndex = 0;
};
} // namespace siren::ecs

#pragma once

#include "ECSProperties.hpp"
#include <typeindex>
#include "core/assert.hpp"

#include "core/Core.hpp"


namespace siren::core
{
struct Component;

/**
 * @brief This class handles assigning each Component Type a unique index in the range [0,
 * MAX_COMPONENTS]. This is used for setting the ComponentMask bits.
 */
class ComponentBitMap {
public:
    template <typename T>
        requires(std::is_base_of_v<Component, T>)
    static size_t get_bit_index() {
        SIREN_ASSERT(
            s_next <= MAX_COMPONENTS,
            "Cannot register more components than MAX_COMPONENTS allows!"
        );

        const auto typeIndex = std::type_index(typeid(T));
        if (!s_registry.contains(typeIndex)) { s_registry[typeIndex] = s_next++; }

        return s_registry[typeIndex];
    }

private:
    static inline std::unordered_map<std::type_index, size_t> s_registry{ };
    static inline size_t s_next = 0;
};
} // namespace siren::ecs

#pragma once

#include "secsAssert.hpp"
#include "secsTypes.hpp"
#include <typeindex>

namespace core::secs
{

/**
 * @brief This class is used by components to make sure they receive a unique ID without much
 * overhead. Note that an id of 0 is considered invalid.
 */
class ComponentIDGenerator
{
public:
    static ComponentID id()
    {
        return s_currentId++;
    }

private:
    static inline ComponentID s_currentId = 1;
};

/**
 * @brief This class handles assigning each Component Type a unique number in the range [0,
 * MAX_COMPONENTS]. This is used for setting the ComponentMask bits.
 */
class ComponentBitRegistry
{
public:
    template <typename T> static size_t index()
    {
        SECS_ASSERT(s_nextIndex <= MAX_COMPONENTS,
                    "Cannot register more components than MAX_COMPONENTS allows!");

        const auto typeIndex = std::type_index(typeid(T));
        if (!s_registry.contains(typeIndex)) { s_registry[typeIndex] = s_nextIndex++; }

        return s_registry[typeIndex];
    }

private:
    static inline hashmap<std::type_index, size_t> s_registry{};
    static inline size_t s_nextIndex = 0;
};

/**
 *@brief The base Component class that all other Components must implement.
 */
struct Component {
    ComponentID id = ComponentIDGenerator::id();

    virtual ~Component() = default;
};

} // namespace core::secs

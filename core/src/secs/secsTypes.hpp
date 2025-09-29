#pragma once

#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace core::secs
{
/// @brief A shared pointer for a given type.
template <typename T> using ref = std::shared_ptr<T>;
template <typename T, typename... Args> ref<T> makeRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

/// @brief A hash map aka std::unordered_map
template <typename K, typename V> using hashmap = std::unordered_map<K, V>;

/// @brief A hash set aka std::unordered_set
template <typename K> using hashset = std::unordered_set<K>;

constexpr int MAX_COMPONENTS = 32;
constexpr int MAX_ENTITIES   = 1000;
using EntityID               = uint32_t;
using ComponentID            = uint32_t;
using ComponentMask          = std::bitset<MAX_COMPONENTS>;

} // namespace core::secs

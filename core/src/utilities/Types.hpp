#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace siren
{

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
Ref<T> makeRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using Uref = std::unique_ptr<T>;

template <typename T, typename... Args>
Uref<T> makeUref(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Wref = std::weak_ptr<T>;

using Byte = uint8_t;

template <typename T>
using Maybe = std::optional<T>;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template <typename K>
using HashSet = std::unordered_set<K>;

constexpr std::nullopt_t Nothing = std::nullopt;

using Path = std::filesystem::path;

} // namespace siren

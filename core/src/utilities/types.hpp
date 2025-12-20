/**
 * @file types.hpp
 * @brief Type aliases for core Siren types
 *
 * The main reasons for aliasing cpp types is code readability, ease of use and the possibility to
 * define custom implementations for any of these core types in the future if the use case arises.
 * Eventually, this may become its own core module.
 */
#pragma once

#include <bitset>
#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>


namespace siren
{
// ============================================================================
// == MARK: Plain Old Data Types
// ============================================================================

/// @brief An unsigned 8-bit integer, aka a byte
using u8 = uint8_t;
/// @brief An unsigned 16-bit integer
using u16 = uint16_t;
/// @brief An unsigned 32-bit integer
using u32 = uint32_t;
/// @brief An unsigned 64-bit integer
using u64 = uint64_t;

/// @brief A signed 8-bit integer
using i8 = int8_t;
/// @brief A signed 16-bit integer
using i16 = int16_t;
/// @brief A signed 32-bit integer
using i32 = int32_t;
/// @brief A signed 63-bit integer
using i64 = int64_t;

/// @brief A byte
using byte = u8;

// ============================================================================
// == MARK: Smart Pointers
// ============================================================================

/// @brief A Siren Reference counted pointer
template <typename T>
using Ref = std::shared_ptr<T>;

/// @brief A function to create a reference counted pointer
template <typename T, typename... Args>
Ref<T> createRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

/// @brief A Siren ownership model pointer
template <typename T>
using Own = std::unique_ptr<T>;

/// @brief A function to create an ownership pointer
template <typename T, typename... Args>
Own<T> createOwn(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

/// @brief A Siren weak pointer
template <typename T>
using Weak = std::weak_ptr<T>;

// ============================================================================
// == MARK: Misc. Types
// ============================================================================

/// @brief An optional value, can either contain a value or @ref Nothing
template <typename T>
using Maybe = std::optional<T>;

/// @brief Constant value representing a value of Nothing for the @ref Maybe type
constexpr std::nullopt_t Nothing = std::nullopt;

/// @brief An unordered hash map
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

/// @brief An unordered hash set
template <typename K>
using HashSet = std::unordered_set<K>;

/// @brief A filesystem path
using Path = std::filesystem::path;

/// @brief A dynamically sized, heap allocated growable container.
template <typename TData>
using Vector = std::vector<TData>;

/// @brief A statically sized container of TData.
template <typename TData, std::size_t size>
using Array = std::array<TData, size>;

/// @brief A tightly packed array of bits.
template <std::size_t N>
using BitSet = std::bitset<N>;
} // namespace siren

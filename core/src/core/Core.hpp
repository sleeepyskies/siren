#pragma once

#include <bitset>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdfloat>


/// @brief The core namespace of siren. Holds all goodies of the engine.
namespace siren::core { }


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
/// @brief An unsigned 128-bit integer
using u128 = __uint128_t;

/// @brief A signed 8-bit integer
using i8 = int8_t;
/// @brief A signed 16-bit integer
using i16 = int16_t;
/// @brief A signed 32-bit integer
using i32 = int32_t;
/// @brief A signed 64-bit integer
using i64 = int64_t;
/// @brief A signed 128-bit integer
using i128 = __int128_t;

/// @brief A 16-bit floating point number.
using f16 = std::float16_t;
/// @brief A 32-bit floating point number.
using f32 = std::float32_t;
/// @brief A 64-bit floating point number.
using f64 = std::float64_t;
/// @brief A 128-bit floating point number.
using f128 = std::float128_t;

/// @brief An unsigned integer capable of holding the largest memory address.
using usize = size_t;

/// @brief A byte.
using byte = u8;

// ============================================================================
// == MARK: Math
// ============================================================================
template <usize Length, typename T>
using Vec = glm::vec<Length, T>;

using Vec2f = glm::vec2;
using Vec3f = glm::vec3;
using Vec4f = glm::vec4;

using Vec2i = glm::ivec2;
using Vec3i = glm::ivec3;
using Vec4i = glm::ivec4;

using Vec2u = glm::uvec2;
using Vec3u = glm::uvec3;
using Vec4u = glm::uvec4;

using Vec2d = glm::dvec2;
using Vec3d = glm::dvec3;
using Vec4d = glm::dvec4;

using Vec2b = glm::bvec2;
using Vec3b = glm::bvec3;
using Vec4b = glm::bvec4;

using Mat2f = glm::mat2;
using Mat3f = glm::mat3;
using Mat4f = glm::mat4;

using RGB  = glm::vec3;
using RGBA = glm::vec4;

// ============================================================================
// == MARK: Smart Pointers
// ============================================================================

/// @brief A Siren Reference counted pointer
template <typename T>
using Ref = std::shared_ptr<T>;

/// @brief A function to create a reference counted pointer
template <typename T, typename... Args>
Ref<T> create_ref(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

/// @brief An owning pointer
template <typename T>
using Own = std::unique_ptr<T>;

/// @brief A function to create an ownership pointer
template <typename T, typename... Args>
Own<T> create_own(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

/// @brief A Siren weak pointer
template <typename T>
using Weak = std::weak_ptr<T>;

// ============================================================================
// == MARK: Misc. Types
// ============================================================================

/// @brief An unordered hash map
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

/// @brief An unordered hash set
template <typename K>
using HashSet = std::unordered_set<K>;

/// @brief A dynamically sized, heap allocated growable container.
template <typename TData>
using Vector = std::vector<TData>;

/// @brief A dynamically sized string.
using String = std::string;

/// @brief A statically sized container of TData.
template <typename TData, std::size_t size>
using Array = std::array<TData, size>;

/// @brief A queue.
template <typename T>
using Queue = std::queue<T>;

/// @brief A tightly packed array of bits.
template <std::size_t N>
using BitSet = std::bitset<N>;

// ============================================================================
// == MARK: Math helpers
// ============================================================================
inline auto create_vec3(const float* value) -> glm::vec3 { return glm::vec3{ value[0], value[1], value[2] }; }
inline auto create_vec4(const float* value) -> glm::vec4 { return glm::vec4{ value[0], value[1], value[2], value[3] }; }

// ============================================================================
// == MARK: Macros
// ============================================================================

/// @brief Macros toggles single threaded mode in siren.
// #define SIREN_SINGLE_THREADED

} // namespace siren

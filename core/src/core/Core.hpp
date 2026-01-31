#pragma once

#include <bitset>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdfloat>
#include <glm/glm.hpp>


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

template <usize Col, usize Row, typename T>
using Mat = glm::mat<Col, Row, T>;

using Vec2f = Vec<2, f32>;
using Vec3f = Vec<3, f32>;
using Vec4f = Vec<4, f32>;

using Vec2i = Vec<2, i32>;
using Vec3i = Vec<3, i32>;
using Vec4i = Vec<4, i32>;

using Vec2u = Vec<2, u32>;
using Vec3u = Vec<3, u32>;
using Vec4u = Vec<4, u32>;

using Vec2d = Vec<2, f64>;
using Vec3d = Vec<3, f64>;
using Vec4d = Vec<4, f64>;

using Vec2b = Vec<2, bool>;
using Vec3b = Vec<3, bool>;
using Vec4b = Vec<4, bool>;

using Mat2f = Mat<2, 2, f32>;
using Mat3f = Mat<3, 3, f32>;
using Mat4f = Mat<4, 4, f32>;

using RGB  = Vec3f;
using RGBA = Vec4f;

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
inline auto create_vec3(const float* value) -> Vec3f { return Vec3f{ value[0], value[1], value[2] }; }
inline auto create_vec4(const float* value) -> Vec4f { return Vec4f{ value[0], value[1], value[2], value[3] }; }

// ============================================================================
// == MARK: Macros
// ============================================================================

/// @brief Macros toggles single threaded mode in siren.
// #define SIREN_SINGLE_THREADED

} // namespace siren

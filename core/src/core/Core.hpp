#pragma once

#include <stdfloat>
#include <glm/glm.hpp>
#include <ranges>


/// @brief The core namespace of siren. Holds all goodies of the engine.
namespace siren::core { }


namespace siren
{
namespace ranges = std::ranges;
namespace views = std::ranges::views;

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

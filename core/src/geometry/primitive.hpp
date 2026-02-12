/**
 * @file primitive.hpp
 * @brief File containing Primitive related data.
 */
#pragma once

#include "../renderer/resources/buffer.hpp"
#include "../renderer/vertex_layout.hpp"

#include "utilities/spch.hpp"


namespace siren::core
{
struct PlaneParams;
struct CapsuleParams;
struct CubeParams;

/**
 * @brief All possible Primitive types as a variant.
*/
using PrimitiveParams = std::variant<
    PlaneParams,
    CapsuleParams,
    CubeParams
>;

/**
 * @brief Plane Primitive parameters.
 */
struct PlaneParams {
    float width        = 1.0f; ///< @brief The depth along the x-axis in [0.f, 1000.f].
    float depth        = 1.0f; ///< @brief The depth along the z-axis in [0.f, 1000.f].
    u32 width_segments = 1;    ///< @brief The amount of segments along the x-axis in [1, 128].
    u32 depth_segments = 1;    ///< @brief The amount of segments along the y-axis in [1, 128].
    auto operator==(const PlaneParams&) const -> bool = default;
};

/**
 * @brief Capsule Primitive parameters.
 */
struct CapsuleParams {
    float radius        = 0.5f; ///< @brief The radius of the capsule in [0.f, 1000.f].
    float height        = 2.0f; ///< @brief The height of the capsule in [0.f, 1000.f].
    u32 capsuleSegments = 16;   ///< @brief The number of segments on each cap in [1, 128].
    u32 radialSegments  = 16;   ///< @brief The amount of segments around the circumference of the capsule in [3, 128].
    u32 heightSegments  = 16;   ///< @brief The amount of segments across the body of the capsule in [1, 128].
    auto operator==(const CapsuleParams&) const -> bool = default;
};

/**
 * @brief Cube Primitive parameters.
 */
struct CubeParams {
    float size          = 1;
    u32 height_segments = 1; // should be at least 1
    u32 width_segments  = 1; // should be at least 1
    u32 depth_segments  = 1; // should be at least 1
    auto operator==(const CubeParams&) const -> bool = default;
};

struct PrimitiveMeshData {
    Buffer vertices;
    Buffer indices;
    usize index_count;
};


namespace primitive
{
// todo: functions shouldn't return a std::shared_ptr<>

/// @brief Generates primitive geometry.
auto generate(const PrimitiveParams& params, const VertexLayout& layout) -> PrimitiveMeshData;
/// @brief Generates plane primitive geometry.
auto generate_plane(const PlaneParams& params, const VertexLayout& layout) -> PrimitiveMeshData;
/// @brief Generates capsule primitive geometry.
auto generate_capsule(const CapsuleParams& params, const VertexLayout& layout) -> PrimitiveMeshData;
/// @brief Generates capsule primitive geometry.
auto generate_cube(const CubeParams& params, const VertexLayout& layout) -> PrimitiveMeshData;

/// @brief Creates a name for the given primitive.
auto create_primitive_name(const PrimitiveParams& params) -> std::string;
} // namespace primitive
} // namespace siren::core

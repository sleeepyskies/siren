/**
 * @file primitive.hpp
 * @brief File containing Primitive related data.
 */
#pragma once

#include "renderer/buffer/VertexArray.hpp"
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
struct PlaneParams
{
    float width       = 1.0f; ///< @brief The depth along the x-axis in [0.f, 1000.f].
    float depth       = 1.0f; ///< @brief The depth along the z-axis in [0.f, 1000.f].
    u32 widthSegments = 1;    ///< @brief The amount of segments along the x-axis in [1, 128].
    u32 depthSegments = 1;    ///< @brief The amount of segments along the y-axis in [1, 128].
};

/**
 * @brief Capsule Primitive parameters.
 */
struct CapsuleParams
{
    float radius        = 0.5f; ///< @brief The radius of the capsule in [0.f, 1000.f].
    float height        = 2.0f; ///< @brief The height of the capsule in [0.f, 1000.f].
    u32 capsuleSegments = 16;   ///< @brief The number of segments on each cap in [1, 128].
    u32 radialSegments  = 16;   ///< @brief The amount of segments around the circumference of the capsule in [3, 128].
    u32 heightSegments  = 16;   ///< @brief The amount of segments across the body of the capsule in [1, 128].
};

struct CubeParams
{
    float size         = 1;
    u32 heightSegments = 1; // should be at least 1
    u32 widthSegments  = 1; // should be at least 1
    u32 depthSegments  = 1; // should be at least 1
};


namespace primitive
{

// todo: functions shouldn't return a Ref<>

/// @brief Generates primitive geometry.
Ref<VertexArray> generate(const PrimitiveParams& params);
/// @brief Generates plane primitive geometry.
Ref<VertexArray> generatePlane(const PlaneParams& params);
/// @brief Generates capsule primitive geometry.
Ref<VertexArray> generateCapsule(const CapsuleParams& params);
/// @brief Generates capsule primitive geometry.
Ref<VertexArray> generateCube(const CubeParams& params);

/// @brief Creates a name for the given primitive.
std::string createPrimitiveName(const PrimitiveParams& params);

} // namespace primitive

} // namespace siren::core

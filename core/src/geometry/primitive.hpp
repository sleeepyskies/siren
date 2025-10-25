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

/// @brief All possible Primitive types as a variant.
using PrimitiveParams = std::variant<PlaneParams, CapsuleParams>;

/**
 * @brief Plane Primitive parameters.
 */
struct PlaneParams {
    float width       = 1.0f;
    float depth       = 1.0f;
    u32 widthSegments = 1; // should be at least 1
    u32 depthSegments = 1; // should be at least 1
};

/**
 * @brief Capsule2D Primitive parameters.
 */
struct CapsuleParams {
    float radius = 0.5f;
    float height = 2.0f;
    u32 segments = 16;
};

namespace primitive
{

/// @brief Generates primitive geometry.
Ref<VertexArray> generate(const PrimitiveParams& params);
/// @brief Generates plane primitive geometry.
Ref<VertexArray> generatePlane(const PlaneParams& params);
/// @brief Generates capsule primitive geometry.
Ref<VertexArray> generateCapsule(const CapsuleParams& params);

/// @brief Creates a name for the given primitive.
std::string createPrimitiveName(const PrimitiveParams& params);

} // namespace primitive

} // namespace siren::core

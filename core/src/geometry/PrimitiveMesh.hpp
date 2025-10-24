/**
 * @file PrimitiveMesh.hpp
 * @brief File containing Primitive related data.
 */
#pragma once

#include "Mesh.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

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

using PrimitiveParams = std::variant<PlaneParams, CapsuleParams>;

/**
 * @brief Represents a basic primitive mesh, meaning some procedural geometric shape.
 */
class PrimitiveMesh final : public Mesh
{
public:
    PrimitiveMesh() = default;
    explicit PrimitiveMesh(const PrimitiveParams& params);
    ~PrimitiveMesh() override = default;

    /// @brief Updates this primitive's paramters, can also change the primitives type. Regenerates
    /// data after setting.
    void setParams(const PrimitiveParams& params);

    /// @brief Generates this primitive's geometric data.
    void generate();

private:
    PrimitiveParams m_params;

    void generatePlane(const PlaneParams& params);
    void generateCapsule(const CapsuleParams& params);
};

} // namespace siren::core

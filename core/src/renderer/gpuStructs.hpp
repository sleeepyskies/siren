/**
 * @file gpuStructs.hpp
 * Contains all GPU structs. A GPu struct has the exact same memory layout as is expected on the GPU.
 */
#pragma once

namespace siren::core
{

/**
 * @brief A GPU correct @ref PointLight.
 * A Siren PointLight is defined by a position, as well as a color.
 */
struct alignas(16) GPUPointLight
{
    float p1, p2, p3; // 12 bytes
    float _pad0 = 0;  // 4 bytes
    float c1, c2, c3; // 12 bytes
    float _pad1 = 0;  // 4 bytes
    // ==> 32 bytes in total
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const GPUPointLight&) const;
};

/**
 * @brief A GPU correct @ref DirectionalLight.
 * A Siren DirectionalLight is defined by a direction, as well as a color.
 */
struct alignas(16) GPUDirectionalLight
{
    float d1, d2, d3; // 12 bytes
    float _pad0 = 0;  // 4 bytes
    float c1, c2, c3; // 12 bytes
    float _pad1 = 0;  // 4 bytes
    // ==> 32 bytes in total
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const GPUDirectionalLight&) const;
};

/**
 * @brief A GPU correct @ref SpotLight.
 * A Siren SpotLight is defined by a position, color, as well as 2 floats for the inner and outer cones.
 */
struct alignas(16) GPUSpotLight
{
    float p1, p2, p3; // 12 bytes
    float inner;      // 4 bytes
    float c1, c2, c3; // 12 bytes
    float outer;      // 4 bytes
    // ==> 32 bytes in total
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const GPUSpotLight&) const;
};


}
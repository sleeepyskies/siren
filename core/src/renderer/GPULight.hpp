/**
 * @file GPULight.hpp
 * Contains all GPU structs. A GPu struct has the exact same memory layout as is expected on the GPU.
 */
#pragma once

#include "utilities/spch.hpp"


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

    bool operator==(const GPUPointLight&) const;

    GPUPointLight() = default;

    GPUPointLight(const glm::vec3& pos, const glm::vec3& col)
        : p1(pos.x), p2(pos.y), p3(pos.z), c1(col.r), c2(col.g), c3(col.b) { }
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

    bool operator==(const GPUDirectionalLight&) const;

    GPUDirectionalLight() = default;

    GPUDirectionalLight(const glm::vec3& dir, const glm::vec3& col)
        : d1(dir.x), d2(dir.y), d3(dir.z), c1(col.r), c2(col.g), c3(col.b) { }
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

    bool operator==(const GPUSpotLight&) const;

    GPUSpotLight() = default;

    GPUSpotLight(const glm::vec3& pos, const glm::vec3& col, const float inner, const float outer)
        : p1(pos.x), p2(pos.y), p3(pos.z), inner(inner), c1(col.r), c2(col.g), c3(col.b), outer(outer) { }
};

static_assert(sizeof(GPUPointLight) == 32);
static_assert(sizeof(GPUDirectionalLight) == 32);
static_assert(sizeof(GPUSpotLight) == 32);
}

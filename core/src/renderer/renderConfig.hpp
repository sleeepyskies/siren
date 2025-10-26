/**
 * @file renderConfig.hpp
 * Contains structs with global render information.
 */
#pragma once

#include "utilities/spch.hpp"
#include "gpuStructs.hpp"

namespace siren::core
{

/// @brief The maximum amount of lights for a light type.
constexpr int MAX_LIGHT_COUNT = 16;

/**
 * @brief Global camera information. Contains the projection-view matrix as well as the position of the camera.
 */
struct CameraInfo
{
    glm::mat4 projectionViewMatrix;
    glm::vec3 position;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const CameraInfo&) const;
};

/**
 * @brief Global light information. Contains arrays of lights divided by type.
 */
struct LightInfo
{
    std::array<GPUPointLight, MAX_LIGHT_COUNT> pointLights{};
    std::array<GPUDirectionalLight, MAX_LIGHT_COUNT> directionalLights{};
    std::array<GPUSpotLight, MAX_LIGHT_COUNT> spotLights{};
    // std::vector<SkyLight> skyLight;
    // std::vector<AreaLight> areaLight;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const LightInfo&) const;
};

/**
 * @note Unused currently.
 */
struct EnvironmentInfo
{
};

/**
 * @brief Global scene information.
 */
struct RenderInfo
{
    CameraInfo cameraInfo;
    LightInfo lightInfo;
    // EnvironmentInfo environmentInfo;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const RenderInfo&) const;
};


}
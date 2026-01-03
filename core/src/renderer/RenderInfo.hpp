/**
 * @file renderConfig.hpp
 * Contains structs with global render information.
 */
#pragma once

#include "Texture.hpp"

#include "utilities/spch.hpp"
#include "GPULight.hpp"


namespace siren::core
{
/// @brief The maximum amount of lights for a light type.
constexpr int MAX_LIGHT_COUNT = 16;

/**
 * @brief Global camera information. Contains the projection-view matrix as well as the position of the camera.
 */
struct CameraInfo
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 position;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const CameraInfo&) const;
};

/**
 * @brief Global light information. Contains arrays of lights divided by type.
 */
struct LightInfo
{
    Array<GPUPointLight, MAX_LIGHT_COUNT> pointLights{ };
    Array<GPUDirectionalLight, MAX_LIGHT_COUNT> directionalLights{ };
    Array<GPUSpotLight, MAX_LIGHT_COUNT> spotLights{ };
    // Vector<AreaLight> areaLight;
    u32 pointLightCount;
    u32 directionalLightCount;
    u32 spotLightCount;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const LightInfo&) const;
};

/**
 * @brief Stores global environment related data.
 */
struct EnvironmentInfo
{
    Ref<TextureCubeMap> skybox = nullptr;

    bool operator==(const EnvironmentInfo&) const;
};

/**
 * @brief Global scene information.
 */
struct RenderInfo
{
    CameraInfo cameraInfo;
    LightInfo lightInfo;
    EnvironmentInfo environmentInfo;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const RenderInfo&) const;
};
}

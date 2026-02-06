/**
 * @file RenderInfo.hpp
 * Contains structs with global render information.
 */
#pragma once

#include "resources/Texture.hpp"

#include "utilities/spch.hpp"
#include "GPULight.hpp"


namespace siren::core
{
/// @brief The maximum amount of lights for a light type.
constexpr i32 MAX_LIGHT_COUNT = 100;
static_assert(MAX_LIGHT_COUNT == 100 && "Make sure to update shader max light count as well.");

/**
 * @brief Global camera information. Contains the projection-view matrix as well as the position of the camera.
 */
struct CameraInfo {
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 position;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const CameraInfo&) const;
};

/**
 * @brief Global light information. Contains arrays of lights divided by type.
 */
struct LightInfo {
    std::array<GPUPointLight, MAX_LIGHT_COUNT> pointLights{ };
    std::array<GPUDirectionalLight, MAX_LIGHT_COUNT> directionalLights{ };
    std::array<GPUSpotLight, MAX_LIGHT_COUNT> spotLights{ };
    // Vector<AreaLight> areaLight;
    u32 pointLightCount       = 0; // we could pack these into a single number aka an u64 or something
    u32 directionalLightCount = 0;
    u32 spotLightCount        = 0;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const LightInfo&) const;
};

/**
 * @brief Stores global environment related data.
 */
struct EnvironmentInfo {
    std::shared_ptr<Texture> skybox = nullptr;

    bool operator==(const EnvironmentInfo&) const;
};

/**
 * @brief Global scene information.
 */
struct RenderInfo {
    CameraInfo cameraInfo;
    LightInfo lightInfo;
    EnvironmentInfo environmentInfo;
    /// @brief Custom compilation operator ensure correctness.
    bool operator==(const RenderInfo&) const;
};
}

#pragma once

#include "buffer/UniformBuffer.hpp"
#include "buffer/VertexArray.hpp"
#include "renderer/material/Material.hpp"
#include "shaders/Shader.hpp"

namespace siren::core
{

constexpr int MAX_LIGHT_COUNT = 16;

struct alignas(16) PointLight {
    glm::vec3 position; // 12 bytes
    float _pad0 = 0;    // 4 bytes
    glm::vec3 color;    // 12 bytes
    float _pad1 = 0;    // 4 bytes

    PointLight(const glm::vec3& position, const glm::vec3& color)
        : position(position), color(color){};
    PointLight() : position({0}), color({0}){};

    bool operator==(const PointLight&) const;
};

struct CameraInfo {
    glm::mat4 projectionViewMatrix;
    glm::vec3 position;

    bool operator==(const CameraInfo&) const;
};

struct LightInfo {

    std::vector<PointLight> pointLights{};
    // std::vector<DirectionalLight> directionalLights;
    // std::vector<SpotLight> spotLights;
    // std::vector<SkyLight> skyLight;
    // std::vector<AreaLight> areaLight;

    bool operator==(const LightInfo&) const;
};

struct EnvironmentInfo {
    bool operator==(const EnvironmentInfo&) const;
};

struct RenderInfo {
    CameraInfo cameraInfo;
    LightInfo lightInfo;
    EnvironmentInfo environmentInfo;

    bool operator==(const RenderInfo&) const;
};

class Renderer
{
public:
    static void init();
    static void shutdown();

    static void begin(const RenderInfo& renderInfo);
    static void end();

    static void draw(const Ref<VertexArray>& vertexArray, const Ref<Material>& material,
                     const glm::mat4& objectTransform);

private:
    const static RenderInfo* s_renderInfo;
    static UniformBuffer* s_lightBuffer;
};

} // namespace siren::core

#pragma once

#include "buffer/VertexArray.hpp"
#include "renderer/material/Material.hpp"
#include "shaders/Shader.hpp"

namespace siren::renderer
{

struct CameraInfo {
    glm::mat4 projectionViewMatrix;
    glm::vec3 position;
};

struct LightInfo {

    // std::vector<PointLight> pointLights;
    // std::vector<DirectionalLight> directionalLights;
    // std::vector<SpotLight> spotLights;
    // std::vector<SkyLight> skyLight;
    // std::vector<AreaLight> areaLight;
};

struct EnvironmentInfo {
};

struct RenderProperties {
    CameraInfo cameraInfo;
    LightInfo lightInfo;
    EnvironmentInfo environmentInfo;
};

class Renderer
{
public:
    static void init();
    static void shutdown();

    static void begin(CameraInfo& cameraInfo);
    static void end();

    static void draw(const Ref<VertexArray>& vertexArray, const Ref<Material>& material,
                     const glm::mat4& objectTransform);

private:
    static CameraInfo* s_cameraInfo;
};

} // namespace siren::renderer

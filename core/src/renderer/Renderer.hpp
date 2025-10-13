#pragma once

#include "buffer/VertexArray.hpp"
#include "renderer/material/Material.hpp"
#include "shaders/Shader.hpp"

namespace siren::renderer
{

struct CameraProperties {
    glm::mat4 projectionViewMatrix;
    glm::vec3 position;
};

class Renderer
{
public:
    static void init();
    static void shutdown();

    static void beginScene(CameraProperties& cameraProps);
    static void endScene();

    // TODO: draw should not take a shader, it should be implicit through the material, however my
    // AssetManager cannot handle this yet
    static void draw(const Ref<VertexArray>& vertexArray, const Ref<Material>& material,
                     const glm::mat4& transform, const Ref<Shader>& shader);

private:
    static CameraProperties* s_cameraProps;
};

} // namespace siren::renderer

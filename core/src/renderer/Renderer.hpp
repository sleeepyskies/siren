#pragma once

#include "Buffer.hpp"
#include "geometry/Camera.hpp"
#include "geometry/Lights.hpp"
#include "geometry/Material.hpp"

namespace siren::renderer
{

// TODO: this is just a temporary solution to hardcode lights
struct SceneDescription {
    Ref<geometry::Camera> camera;
    Ref<UniformBuffer> pointLights = nullptr;
};

class Renderer
{
public:
    static void init();
    static void shutdown();

    static void beginScene(const SceneDescription& sceneDescription);
    static void endScene();

    // TODO: draw should not take a shader, it should be implicit through the material, however my
    // AssetManager cannot handle this yet
    static void draw(const Ref<VertexArray>& vertexArray, const Ref<geometry::Material>& material,
                     const glm::mat4& transform, const Ref<Shader>& shader);

private:
    static Uref<SceneDescription> m_sceneDescription;
};

} // namespace siren::renderer

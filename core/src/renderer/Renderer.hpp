#pragma once

#include "Buffer.hpp"
#include "geometry/Camera.hpp"
#include "geometry/Material.hpp"

namespace core::renderer
{

class Renderer
{
public:
    static void init();
    static void shutdown();

    static void beginScene(const Camera& camera);
    static void endScene();

    // TODO: draw should not take a shader, it should be implicit through the material, however my
    // AssetManager cannot handle this yet
    static void draw(const Ref<VertexArray>& vertexArray, const Ref<geometry::Material>& material,
                     const glm::mat4& transform, const Ref<Shader>& shader);
};

} // namespace core::renderer

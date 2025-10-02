#pragma once

#include "Buffer.hpp"
#include "geometry/Material.hpp"

namespace core::renderer
{

class Renderer
{
public:
    static void init();
    static void shutdown();

    static void beginScene();
    static void endScene();

    static void submit(const Ref<VertexArray>& VAO, const Ref<geometry::Material>& material,
                       const glm::mat4& transform);
};

} // namespace core::renderer

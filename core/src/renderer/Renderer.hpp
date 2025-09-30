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

    static void submit(const ref<VertexArray>& VAO, const Material& material,
                       const glm::mat4& transform);
};

} // namespace core::renderer

#include "Renderer.hpp"

namespace core::renderer
{

void Renderer::init()
{
    // any setup logic here, init context in future??
}

void Renderer::shutdown()
{
    // nothing for now
}

void Renderer::beginScene()
{
    // um??
}

void Renderer::endScene()
{
    // maybe could batch draw calls here??
}

void Renderer::submit(const ref<VertexArray>& VAO, const Material& material,
                      const glm::mat4& transform)
{
    // set material uniforms, check if draw tris or elements
}

} // namespace core::renderer

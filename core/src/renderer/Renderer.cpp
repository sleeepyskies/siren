#include "Renderer.hpp"

namespace core::renderer
{

void Renderer::init()
{
    // setup graphics context, could be put into api agnostic class
}

void Renderer::shutdown()
{
}

void Renderer::beginScene()
{
}

void Renderer::endScene()
{
}

void Renderer::submit(const VertexArray& va, const Shader& shader, const glm::mat4& transform)
{
}

} // namespace core::renderer

#include "Renderer.hpp"

namespace core::renderer
{

void Renderer::init()
{
    // api context in future??
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
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

void Renderer::submit(const Ref<VertexArray>& VAO, const Ref<geometry::Material>& material,
                      const glm::mat4& transform)
{
    // set material uniforms, check if draw tris or elements
}

} // namespace core::renderer

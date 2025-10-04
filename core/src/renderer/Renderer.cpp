#include "Renderer.hpp"

#include "geometry/Camera.hpp"

namespace core::renderer
{

static const Camera* s_cam = nullptr;

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

void Renderer::beginScene(const Camera& camera)
{
    s_cam = &camera; // TODO: this is a bad solution lol, but works for now
}

void Renderer::endScene()
{
    s_cam = nullptr;
}

void Renderer::draw(const Ref<VertexArray>& vertexArray, const Ref<geometry::Material>& material,
                    const glm::mat4& transform, const Ref<Shader>& shader)
{
    shader->bind();

    // required uniforms
    shader->setUniformMat4("uProjView", s_cam->projectionViewMatrix());
    shader->setUniformMat4("uModel", transform);
    shader->setUniformVec3("uCameraPos", s_cam->position());

    // set material values with non-affecting defaults
    shader->setUniformVec4("uBaseColorFactor", material->baseColorFactor);
    shader->setUniformFloat("uMetallicFactor", material->metallicFactor);
    shader->setUniformFloat("uRoughnessFactor", material->roughnessFactor);
    shader->setUniformVec3("uEmissionColor", material->emissionColor);
    shader->setUniformFloat("uOcclusionStrength", material->occlusionStrength);
    shader->setUniformFloat("uNormalScale", material->normalScale);

    int slot = 0;

    // optional material uniforms
    if (material->baseColorMap) {
        material->baseColorMap->bind(slot);
        shader->setUniformImage("uBaseColorMap", slot++);
    }
    if (material->metallicRoughnessMap) {
        material->metallicRoughnessMap->bind(slot);
        shader->setUniformImage("uMetallicRoughnessMap", slot++);
    }
    if (material->emissionMap) {
        material->emissionMap->bind(slot);
        shader->setUniformImage("uEmissionMap", slot++);
    }
    if (material->occlusionMap) {
        material->occlusionMap->bind(slot);
        shader->setUniformImage("uOcclusionMap", slot++);
    }
    if (material->normalMap) {
        material->normalMap->bind(slot);
        shader->setUniformImage("uNormalMap", slot++);
    }

    // uniform flag for material params and VertexArray attributes
    uint32_t materialFlags = 0;
    if (vertexArray->hasAttribute(AllowedShaderAttribute::COLOR)) { materialFlags |= 1 << 0; }
    if (vertexArray->hasAttribute(AllowedShaderAttribute::NORMAL)) { materialFlags |= 1 << 1; }
    if (vertexArray->hasAttribute(AllowedShaderAttribute::UV)) { materialFlags |= 1 << 2; }

    if (material->baseColorMap) { materialFlags |= 1 << 3; }
    if (material->metallicRoughnessMap) { materialFlags |= 1 << 4; }
    if (material->emissionMap) { materialFlags |= 1 << 5; }
    if (material->occlusionMap) { materialFlags |= 1 << 6; }
    if (material->normalMap) { materialFlags |= 1 << 7; }

    shader->setUniformUnsignedInt("uMaterialFlags", materialFlags);

    const GLenum indexType =
        static_cast<GLenum>(vertexArray->getIndexBuffer()->getIndexType().type);
    const int indexCount = vertexArray->getIndexBuffer()->getIndexCount();

    vertexArray->bind();
    glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
    vertexArray->unbind();
}

} // namespace core::renderer

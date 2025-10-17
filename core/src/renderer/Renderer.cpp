#include "Renderer.hpp"

#include "renderer/material/MaterialKey.hpp"

#include <core/Application.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace siren::renderer
{

CameraInfo* Renderer::s_cameraInfo = nullptr;

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

void Renderer::begin(CameraInfo& cameraInfo)
{
    s_cameraInfo = &cameraInfo;
}

void Renderer::end()
{
    s_cameraInfo = nullptr;
}

void Renderer::draw(const Ref<VertexArray>& vertexArray, const Ref<Material>& material,
                    const glm::mat4& objectTransform)
{
    // TODO: default material?
    if (!material) { return; }
    if (!material->shaderHandle) { return; }
    const auto& shader =
        core::Application::get().getAssetManager().getAsset<Shader>(material->shaderHandle);

    shader->bind();

    // required uniforms
    shader->setUniformMat4("uProjView", s_cameraInfo->projectionViewMatrix);
    shader->setUniformMat4("uModel", objectTransform);
    shader->setUniformVec3("uCameraPos", s_cameraInfo->position);

    // set material values with non-affecting defaults
    shader->setUniformVec4("uBaseColorFactor", material->baseColor);
    shader->setUniformFloat("uMetallicFactor", material->metallic);
    shader->setUniformFloat("uRoughnessFactor", material->roughness);
    shader->setUniformVec3("uEmissionColor", material->emission);
    shader->setUniformFloat("uOcclusionStrength", material->occlusion);
    shader->setUniformFloat("uNormalScale", material->normalScale);

    int slot = 0;

    // optional material uniforms
    if (material->hasTexture(TextureType::BASE_COLOR)) {
        material->getTexture(TextureType::BASE_COLOR)->attach(slot);
        shader->setUniformTexture2D("uBaseColorMap", slot++);
    }
    if (material->hasTexture(TextureType::METALLIC)) {
        material->getTexture(TextureType::METALLIC)->attach(slot);
        shader->setUniformTexture2D("uMetallicMap", slot++);
    }
    if (material->hasTexture(TextureType::ROUGHNESS)) {
        material->getTexture(TextureType::ROUGHNESS)->attach(slot);
        shader->setUniformTexture2D("uRoughnessMap", slot++);
    }
    if (material->hasTexture(TextureType::EMISSION)) {
        material->getTexture(TextureType::EMISSION)->attach(slot);
        shader->setUniformTexture2D("uEmissionMap", slot++);
    }
    if (material->hasTexture(TextureType::OCCLUSION)) {
        material->getTexture(TextureType::OCCLUSION)->attach(slot);
        shader->setUniformTexture2D("uOcclusionMap", slot++);
    }
    if (material->hasTexture(TextureType::NORMAL)) {
        material->getTexture(TextureType::NORMAL)->attach(slot);
        shader->setUniformTexture2D("uNormalMap", slot++);
    }

    // uniform flag for material params and VertexArray attributes
    uint32_t materialFlags = 0;
    if (material->hasTexture(TextureType::BASE_COLOR)) { materialFlags |= 1 << 0; }
    if (material->hasTexture(TextureType::METALLIC)) { materialFlags |= 1 << 1; }
    if (material->hasTexture(TextureType::ROUGHNESS)) { materialFlags |= 1 << 2; }
    if (material->hasTexture(TextureType::EMISSION)) { materialFlags |= 1 << 3; }
    if (material->hasTexture(TextureType::OCCLUSION)) { materialFlags |= 1 << 4; }
    if (material->hasTexture(TextureType::NORMAL)) { materialFlags |= 1 << 5; }

    shader->setUniformUnsignedInt("uMaterialFlags", materialFlags);

    const GLenum indexType = vertexArray->getIndexBuffer()->getIndexType();
    const int indexCount   = vertexArray->getIndexBuffer()->getIndexCount();

    vertexArray->bind();
    glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
    vertexArray->unbind();
}

} // namespace siren::renderer

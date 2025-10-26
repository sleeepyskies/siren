// ReSharper disable CppVariableCanBeMadeConstexpr
#include "RenderModule.hpp"

#include "buffer/UniformBuffer.hpp"
#include "renderer/material/MaterialKey.hpp"

#include <core/Application.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace siren::core
{

bool RenderModule::initialize()
{
    // api context in future??
    glEnable(GL_DEPTH_TEST); // enable the depth testing stage in the pipeline
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // allow writing to the depth buffer
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    return true;
}

void RenderModule::shutdown()
{
    // nothing for now
}

void RenderModule::begin(const RenderInfo& renderInfo)
{
    if (m_renderInfo == renderInfo) {
        return;
    }

    m_renderInfo = renderInfo;
    setupCamera();
    setupLights();
}

void RenderModule::end()
{
    // handle batching here?
}


void RenderModule::setupCamera()
{
}

void RenderModule::setupLights()
{
    m_lightBuffer = nullptr;
    std::vector<u8> bytes{};

    auto writeToBuffer = [](const void* src, std::vector<u8>& dest, const u32 size) -> void {
        auto* srcBytes = static_cast<const u8*>(src);
        dest.insert(dest.end(), srcBytes, srcBytes + size);
    };

    // copy light data

    writeToBuffer(
        m_renderInfo.lightInfo.pointLights.data(),
        bytes,
        sizeof(GPUPointLight) * MAX_LIGHT_COUNT
        );

    writeToBuffer(
        m_renderInfo.lightInfo.directionalLights.data(),
        bytes,
        sizeof(GPUDirectionalLight) * MAX_LIGHT_COUNT
        );

    writeToBuffer(
        m_renderInfo.lightInfo.spotLights.data(),
        bytes,
        sizeof(GPUSpotLight) * MAX_LIGHT_COUNT
        );

    // write light counts
    const u32 pointLightCount       = m_renderInfo.lightInfo.pointLights.size();
    const u32 directionalLightCount = m_renderInfo.lightInfo.directionalLights.size();
    const u32 spotLightCount        = m_renderInfo.lightInfo.spotLights.size();
    writeToBuffer(&pointLightCount, bytes, sizeof(pointLightCount));
    writeToBuffer(&directionalLightCount, bytes, sizeof(directionalLightCount));
    writeToBuffer(&spotLightCount, bytes, sizeof(spotLightCount));

    m_lightBuffer = createOwn<UniformBuffer>(bytes, BufferUsage::STATIC);

    // we only support point lights atm
    m_lightBuffer->attach(1);
}


void RenderModule::draw(const Ref<VertexArray>& vertexArray,
                        const Ref<Material>& material,
                        const glm::mat4& objectTransform)
{
    // todo:
    //  - default material?
    //  - all these bindings should happen inside shader

    if (!material) {
        return;
    }
    if (!material->shaderHandle) {
        return;
    }
    const auto& shader =
        core::App::get().getAssetManager().getAsset<Shader>(material->shaderHandle);

    const auto& [projectionViewMatrix, cameraPosition] = s_renderInfo->cameraInfo;
    const EnvironmentInfo& environmentInfo             = s_renderInfo->environmentInfo;

    shader->bind();

    // required uniforms
    shader->setUniformMat4("uProjView", projectionViewMatrix);
    shader->setUniformMat4("uModel", objectTransform);
    shader->setUniformVec3("uCameraPos", cameraPosition);

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
    if (material->hasTexture(TextureType::BASE_COLOR)) {
        materialFlags |= 1 << 0;
    }
    if (material->hasTexture(TextureType::METALLIC)) {
        materialFlags |= 1 << 1;
    }
    if (material->hasTexture(TextureType::ROUGHNESS)) {
        materialFlags |= 1 << 2;
    }
    if (material->hasTexture(TextureType::EMISSION)) {
        materialFlags |= 1 << 3;
    }
    if (material->hasTexture(TextureType::OCCLUSION)) {
        materialFlags |= 1 << 4;
    }
    if (material->hasTexture(TextureType::NORMAL)) {
        materialFlags |= 1 << 5;
    }

    shader->setUniformUnsignedInt("uMaterialFlags", materialFlags);

    const GLenum indexType = vertexArray->getIndexBuffer()->getIndexType();
    const int indexCount   = vertexArray->getIndexBuffer()->getIndexCount();

    vertexArray->bind();
    glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
    vertexArray->unbind();
}

} // namespace siren::core
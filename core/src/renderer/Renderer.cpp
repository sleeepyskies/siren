#include "Renderer.hpp"

#include "buffer/UniformBuffer.hpp"
#include "renderer/material/MaterialKey.hpp"

#include <core/Application.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace siren::renderer
{

const RenderInfo* Renderer::s_renderInfo = nullptr;
UniformBuffer* Renderer::s_lightBuffer   = nullptr;

bool PointLight::operator==(const PointLight& other) const
{
    return position == other.position && color == other.color;
}

bool CameraInfo::operator==(const CameraInfo& other) const
{
    return position == other.position && projectionViewMatrix == other.projectionViewMatrix;
}

bool LightInfo::operator==(const LightInfo& other) const
{
    if (pointLights.size() != other.pointLights.size()) { return false; }
    for (size_t i = 0; i < pointLights.size(); i++) {
        if (pointLights[i] != other.pointLights[i]) { return false; }
    }

    return true;
}

bool EnvironmentInfo::operator==(const EnvironmentInfo&) const
{
    // todo:
    return true;
}

bool RenderInfo::operator==(const RenderInfo& other) const
{
    return cameraInfo == other.cameraInfo && environmentInfo == other.environmentInfo &&
           lightInfo == other.lightInfo;
}

void Renderer::init()
{
    // api context in future??
    glEnable(GL_DEPTH_TEST); // enable the depth testing stage in the pipeline
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // allow writing to the depth buffer
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void Renderer::shutdown()
{
    // nothing for now
}

void Renderer::begin(const RenderInfo& renderInfo)
{
    // todo: add camera data into UniformBuffer instead of inside of draw

    if (!s_renderInfo || *s_renderInfo != renderInfo) {
        s_renderInfo = &renderInfo;

        if (s_lightBuffer) { delete s_lightBuffer; }
        std::vector<Byte> rawLightData{};

        const int lightCount =
            std::min(MAX_LIGHT_COUNT, static_cast<int>(renderInfo.lightInfo.pointLights.size()));

        for (size_t i = 0; i < lightCount; i++) {
            const auto& pl   = s_renderInfo->lightInfo.pointLights[i];
            const auto* data = reinterpret_cast<const Byte*>(&pl);
            rawLightData.insert(rawLightData.end(), data, data + sizeof(PointLight));
        }

        for (int i = lightCount; i < MAX_LIGHT_COUNT; i++) {
            PointLight dummy{}; // Zero-initialized
            const auto* data = reinterpret_cast<const Byte*>(&dummy);
            rawLightData.insert(rawLightData.end(), data, data + sizeof(PointLight));
        }

        const auto* plCountData = reinterpret_cast<const Byte*>(&lightCount);
        rawLightData.insert(rawLightData.end(), plCountData, plCountData + sizeof(int));
        const int padding[3] = { 0, 0, 0 };
        const auto* padData  = reinterpret_cast<const Byte*>(padding);
        rawLightData.insert(rawLightData.end(), padData, padData + sizeof(padding));

        s_lightBuffer = new UniformBuffer(rawLightData, BufferUsage::STATIC);

        // we only support point lights atm
        s_lightBuffer->attach(0);
    }
}

void Renderer::end()
{
    // handle batching here?
}

void Renderer::draw(const Ref<VertexArray>& vertexArray, const Ref<Material>& material,
                    const glm::mat4& objectTransform)
{
    // todo:
    //  - default material?
    //  - all these bindings should happen inside shader

    if (!material) { return; }
    if (!material->shaderHandle) { return; }
    const auto& shader =
        core::Application::get().getAssetManager().getAsset<Shader>(material->shaderHandle);

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

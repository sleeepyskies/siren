// ReSharper disable CppVariableCanBeMadeConstexpr
#include "RenderModule.hpp"

#include "assets/AssetModule.hpp"
#include "buffer/UniformBuffer.hpp"
#include "shaders/Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace siren::core
{

/// @brief Helper function for copying to a buffer.
static auto writeToBuffer = [](const void* src, std::vector<u8>& dest, const u32 size) -> void {
    auto* srcBytes = static_cast<const u8*>(src);
    dest.insert(dest.end(), srcBytes, srcBytes + size);
};

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
    if (m_drawQueue.empty()) {
        return;
    }
    // then, we sort draw commands to reduce amount of shader changes
    // we make use of AssetHandles just being u64's
    auto sortFn = [](const DrawCommand& d1, const DrawCommand& d2) -> bool {
        return d1.material->shaderHandle < d2.material->shaderHandle;
    };
    std::sort(m_drawQueue.begin(), m_drawQueue.end(), sortFn);

    // used to only switch shader when needed
    AssetHandle currentShader = m_drawQueue.begin()->material->shaderHandle;

    for (const auto& cmd : m_drawQueue) {
        // if we have a new shader, switch
        if (cmd.material->shaderHandle != currentShader) {
            currentShader             = cmd.material->shaderHandle;
            const Ref<Shader>& shader = assets().getAsset<Shader>(currentShader);
            if (!shader) {
                continue;
            }
            shader->bind();
        }

        const Ref<Shader>& shader = assets().getAsset<Shader>(currentShader);

        // bind the material
        bindMaterial(cmd.material);

        shader->setUniformMat4("uModel", cmd.modelTransform);

        const GLenum indexType = cmd.vertexArray->getIndexBuffer()->getIndexType();
        const int indexCount   = cmd.vertexArray->getIndexBuffer()->getIndexCount();

        cmd.vertexArray->bind();
        glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
        cmd.vertexArray->unbind();
    }

    m_drawQueue.clear();
}


void RenderModule::setupCamera()
{
    m_cameraBuffer = nullptr;
    std::vector<u8> bytes{};

    writeToBuffer(
        glm::value_ptr(m_renderInfo.cameraInfo.projectionViewMatrix),
        bytes,
        sizeof(glm::mat4)
        );

    writeToBuffer(
        glm::value_ptr(m_renderInfo.cameraInfo.position),
        bytes,
        sizeof(glm::vec3)
        );

    m_cameraBuffer = createOwn<UniformBuffer>(bytes, BufferUsage::STATIC);
    // rebind each time we change the buffer
    m_cameraBuffer->attach(0);
}

void RenderModule::bindMaterial(const Ref<Material>& material)
{
    if (!material) {
        wrn("Cannot bind nullptr Material!");
        return;
    }
    AssetModule& am = assets();

    const auto shader = am.getAsset<Shader>(material->shaderHandle);

    // render flags
    if (material->doubleSided) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
    }

    // set pbr params
    shader->setUniformVec4("uBaseColorFactor", material->baseColor);
    shader->setUniformFloat("uMetallicFactor", material->metallic);
    shader->setUniformFloat("uRoughnessFactor", material->roughness);
    shader->setUniformVec3("uEmissionColor", material->emissive);
    shader->setUniformFloat("uOcclusionStrength", material->ambientOcclusion);
    shader->setUniformFloat("uNormalScale", material->normalScale);
    shader->setUniformInt("uAlphaMode", static_cast<i32>(material->alphaMode)); // fixme
    shader->setUniformFloat("uAlphaCutoff", material->alphaCutoff);

    u32 slot          = 0;
    u32 materialFlags = 0;

    // todo: lambda + loop ? would need mapping of TextureType to string

    {
        const auto textureHandle = material->getTexture(Material::TextureType::BASE_COLOR);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uBaseColorMap", slot++);
            materialFlags |= 1 << 0;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::METALLIC_ROUGHNESS);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uMetallicRoughnessMap", slot++);
            materialFlags |= 1 << 1;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::EMISSION);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uEmissionMap", slot++);
            materialFlags |= 1 << 2;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::OCCLUSION);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uOcclusionMap", slot++);
            materialFlags |= 1 << 3;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::NORMAL);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uNormalMap", slot++);
            materialFlags |= 1 << 4;
        }
    }

    shader->setUniformUnsignedInt("uMaterialFlags", materialFlags);
}


void RenderModule::setupLights()
{
    m_lightBuffer = nullptr;
    std::vector<u8> bytes{};

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
    // rebind each time we change the buffer
    m_lightBuffer->attach(1);
}


void RenderModule::submit(
    const Ref<VertexArray>& vertexArray,
    const Ref<Material>& material,
    const glm::mat4& objectTransform
    )
{
    if (!vertexArray || !material) {
        return;
    }
    m_drawQueue.emplace_back(vertexArray, material, objectTransform);
}

} // namespace siren::core
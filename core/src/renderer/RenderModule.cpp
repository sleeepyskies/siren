// ReSharper disable CppVariableCanBeMadeConstexpr
#include "RenderModule.hpp"

#include "assets/AssetModule.hpp"
#include "buffer/UniformBuffer.hpp"
#include "shaders/Shader.hpp"
#include "assets/AssetModule.tpp"

#include <glm/gtc/type_ptr.hpp>


namespace siren::core
{

bool RenderModule::initialize()
{
    // api context in future??
    glEnable(GL_DEPTH_TEST); // enable the depth testing stage in the pipeline
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE); // allow writing to the depth buffer
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // init to null data
    m_cameraBuffer = createOwn<UniformBuffer>(
        std::vector<u8>(sizeof(CameraInfo)),
        BufferUsage::DYNAMIC
    );
    m_cameraBuffer->attach(0);
    m_lightBuffer = createOwn<UniformBuffer>(
        std::vector<u8>(sizeof(LightInfo)),
        BufferUsage::STATIC
    );
    m_cameraBuffer->attach(1);

    return true;
}

void RenderModule::shutdown()
{
    // nothing for now
}

void RenderModule::begin(const RenderInfo& renderInfo)
{
    m_renderInfo = renderInfo;
    setupCamera();
    setupLights();

    if (m_frameBuffer) {
        m_frameBuffer->bind();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

// todo: normal matrix??? i dont rlly know about it soo....

void RenderModule::end()
{
    if (m_drawQueue.empty()) {
        return;
    }
    // then, we sort draw commands to reduce amount of shader changes
    // we make use of AssetHandles just being u64's
    auto sortFn = [] (const DrawCommand& d1, const DrawCommand& d2) -> bool
    {
        return d1.material->shaderHandle < d2.material->shaderHandle;
    };
    std::sort(m_drawQueue.begin(), m_drawQueue.end(), sortFn);

    AssetModule& am = assets();

    // used to only switch shader when needed
    AssetHandle currentShaderHandle = m_drawQueue.begin()->material->shaderHandle;
    Ref<Shader> currentShaderRef    = am.getAsset<Shader>(currentShaderHandle);
    currentShaderRef->bind();
    m_stats.shaderBinds++;

    for (const auto& [vertexArray, material, modelTransform] : m_drawQueue) {
        // if we have a new shader, switch
        if (material->shaderHandle != currentShaderHandle) {
            currentShaderHandle = material->shaderHandle;
            currentShaderRef    = am.getAsset<Shader>(currentShaderHandle);
            if (!currentShaderRef) {
                continue;
            }
            currentShaderRef->bind();
            m_stats.shaderBinds++;
        }

        const Ref<Shader>& shader = assets().getAsset<Shader>(currentShaderHandle);

        // bind the material
        bindMaterial(material);

        shader->setUniformMat4("uModel", modelTransform);

        const GLenum indexType = vertexArray->getIndexBuffer()->getIndexType();
        const int indexCount   = vertexArray->getIndexBuffer()->getIndexCount();

        vertexArray->bind();
        glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
        m_stats.drawCalls++;
        m_stats.vertices += indexCount;
        m_stats.triangles += indexCount / 3;
        vertexArray->unbind();
    }

    m_drawQueue.clear();
}


void RenderModule::setupCamera()
{
    struct alignas(16) CameraUBO
    {
        glm::mat4 projectionView;
        glm::vec3 position;
        float _pad;
    } cameraUbo;

    cameraUbo.projectionView = m_renderInfo.cameraInfo.projectionViewMatrix;
    cameraUbo.position       = m_renderInfo.cameraInfo.position;
    m_cameraBuffer->setData(&cameraUbo, sizeof(CameraUBO), BufferUsage::DYNAMIC);
    // rebind each time we change the buffer
    m_cameraBuffer->attach(0);
}

void RenderModule::setupLights()
{
    struct alignas(16) LightUBO
    {
        std::array<GPUPointLight, MAX_LIGHT_COUNT> pointLights;
        std::array<GPUDirectionalLight, MAX_LIGHT_COUNT> directionalLights;
        std::array<GPUSpotLight, MAX_LIGHT_COUNT> spotLights;
        u32 pointLightCount;
        u32 directionalLightCount;
        u32 spotLightCount;
        u32 _pad; // 16-byte alignment
    } lightUbo;

    lightUbo.pointLights           = m_renderInfo.lightInfo.pointLights;
    lightUbo.directionalLights     = m_renderInfo.lightInfo.directionalLights;
    lightUbo.spotLights            = m_renderInfo.lightInfo.spotLights;
    lightUbo.pointLightCount       = static_cast<u32>(m_renderInfo.lightInfo.pointLights.size());
    lightUbo.directionalLightCount = static_cast<u32>(m_renderInfo.lightInfo.directionalLights.
                                                                   size());
    lightUbo.spotLightCount = static_cast<u32>(m_renderInfo.lightInfo.spotLights.size());

    m_lightBuffer->setData(&lightUbo, sizeof(LightUBO), BufferUsage::STATIC);
    m_lightBuffer->attach(1);
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
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    switch (material->alphaMode) {
        case Material::AlphaMode::OPAQUE: {
            glDisable(GL_BLEND);
            break;
        }
        case Material::AlphaMode::BLEND: {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        case Material::AlphaMode::MASK: {
            glDisable(GL_BLEND);
            break;
        }
    }

    // set pbr params
    shader->setUniformVec4("uBaseColorFactor", material->baseColor);
    shader->setUniformFloat("uMetallicFactor", material->metallic);
    shader->setUniformFloat("uRoughnessFactor", material->roughness);
    shader->setUniformVec3("uEmissionColor", material->emissive);
    shader->setUniformFloat("uOcclusionStrength", material->ambientOcclusion);
    shader->setUniformFloat("uNormalScale", material->normalScale);
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
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::METALLIC_ROUGHNESS);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uMetallicRoughnessMap", slot++);
            materialFlags |= 1 << 1;
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::EMISSION);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uEmissionMap", slot++);
            materialFlags |= 1 << 2;
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::OCCLUSION);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uOcclusionMap", slot++);
            materialFlags |= 1 << 3;
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::NORMAL);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("uNormalMap", slot++);
            materialFlags |= 1 << 4;
            m_stats.textureBinds++;
        }
    }

    shader->setUniformUnsignedInt("uMaterialFlags", materialFlags);
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

void RenderModule::setFrameBuffer(const Ref<FrameBuffer>& frameBuffer)
{
    m_frameBuffer = frameBuffer;
}

void RenderModule::clearBuffers(const glm::vec4& color) const
{
    if (m_frameBuffer) {
        m_frameBuffer->bind();
    }
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

const RenderStats& RenderModule::getStats() const
{
    return m_stats;
}

} // namespace siren::core

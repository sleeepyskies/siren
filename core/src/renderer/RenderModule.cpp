// ReSharper disable CppVariableCanBeMadeConstexpr
#define GLM_ENABLE_EXPERIMENTAL
#include "RenderModule.hpp"

#include "assets/AssetModule.hpp"
#include "buffer/UniformBuffer.hpp"
#include "shaders/Shader.hpp"
#include "assets/AssetModule.tpp"

#include <glm/gtx/string_cast.hpp>
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
    m_lightBuffer->attach(1);

    return true;
}

void RenderModule::shutdown()
{
    // nothing for now
}

void RenderModule::beginFrame(const RenderInfo& renderInfo)
{
    m_stats.reset();
    m_renderInfo = renderInfo;

    setupCamera();
    setupLights();
}

// todo: normal matrix??? i dont rlly know about it soo....

void RenderModule::endFrame()
{
    if (m_drawQueue.empty()) {
        FrameBuffer::unbind();
        return;
    }
    // then, we sort draw commands to reduce amount of shader changes
    // we make use of AssetHandles just being u64's
    auto sortFn = [] (const DrawCommand& d1, const DrawCommand& d2) -> bool {
        const u32 id1 = d1.target ? d1.target->getId() : 0;
        const u32 id2 = d2.target ? d2.target->getId() : 0;

        if (id1 != id2) {
            return id1 < id2;
        }

        return d1.material->shaderHandle < d2.material->shaderHandle;
    };
    std::sort(m_drawQueue.begin(), m_drawQueue.end(), sortFn);

    AssetModule& am = assets();

    // used to only switch shader when needed
    AssetHandle currentShaderHandle     = AssetHandle::invalid();
    Ref<Shader> currentShaderRef        = nullptr;
    Ref<FrameBuffer> currentFramebuffer = nullptr;

    for (const auto& [target, vertexArray, material, modelTransform] : m_drawQueue) {
        if (!vertexArray || vertexArray->getIndexBuffer()->getIndexCount() == 0) {
            wrn("Invalid VertexArray when rendering!");
            continue;
        }

        // FrameBuffer change
        if (target != currentFramebuffer) {
            currentFramebuffer = target;
            if (currentFramebuffer) {
                currentFramebuffer->bind();
                currentFramebuffer->setViewport();
            } else {
                FrameBuffer::unbind();
            }
        }

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

    FrameBuffer::unbind();
    m_currentFramebuffer = nullptr;
    m_drawQueue.clear();
}

void RenderModule::beginPass(const Ref<FrameBuffer>& frameBuffer, const glm::vec4& clearColor)
{
    m_currentFramebuffer = frameBuffer;
    if (m_currentFramebuffer) {
        frameBuffer->bind();
        frameBuffer->setViewport();
    } else {
        FrameBuffer::unbind();
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
    m_drawQueue.emplace_back(m_currentFramebuffer, vertexArray, material, objectTransform);
}

const RenderStats& RenderModule::getStats() const
{
    return m_stats;
}

// ReSharper disable once CppMemberFunctionMayBeConst
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
    lightUbo.pointLightCount       = m_renderInfo.lightInfo.pointLightCount;
    lightUbo.directionalLightCount = m_renderInfo.lightInfo.directionalLightCount;
    lightUbo.spotLightCount        = m_renderInfo.lightInfo.spotLightCount;

    m_lightBuffer->setData(&lightUbo, sizeof(LightUBO), BufferUsage::STATIC);
    m_lightBuffer->attach(1);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void RenderModule::setupCamera()
{
    struct alignas(16) CameraUBO
    {
        glm::mat4 projectionView;
        glm::vec3 cameraPosition;
        float _pad;
    } cameraUbo;

    cameraUbo.projectionView = m_renderInfo.cameraInfo.projectionViewMatrix;
    cameraUbo.cameraPosition = m_renderInfo.cameraInfo.position;
    m_cameraBuffer->setData(&cameraUbo, sizeof(CameraUBO), BufferUsage::DYNAMIC);
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
    shader->setUniformVec4("u_baseColorFactor", material->baseColor);
    shader->setUniformFloat("u_metallicFactor", material->metallic);
    shader->setUniformFloat("u_roughnessFactor", material->roughness);
    shader->setUniformVec3("u_emissionColor", material->emissive);
    shader->setUniformFloat("u_occlusionStrength", material->ambientOcclusion);
    shader->setUniformFloat("u_normalScale", material->normalScale);
    shader->setUniformFloat("u_alphaCutoff", material->alphaCutoff);

    u32 slot          = 0;
    u32 materialFlags = 0;

    // todo: lambda + loop ? would need mapping of TextureType to string

    {
        const auto textureHandle = material->getTexture(Material::TextureType::BASE_COLOR);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("u_baseColorMap", slot++);
            materialFlags |= 1 << 0;
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::METALLIC_ROUGHNESS);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("u_metallicRoughnessMap", slot++);
            materialFlags |= 1 << 1;
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::EMISSION);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("u_emissionMap", slot++);
            materialFlags |= 1 << 2;
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::OCCLUSION);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("u_occlusionMap", slot++);
            materialFlags |= 1 << 3;
            m_stats.textureBinds++;
        }
    }
    {
        const auto textureHandle = material->getTexture(Material::TextureType::NORMAL);
        if (textureHandle) {
            const auto texture = am.getAsset<Texture2D>(*textureHandle);
            texture->attach(slot);
            shader->setUniformTexture2D("u_normalMap", slot++);
            materialFlags |= 1 << 4;
            m_stats.textureBinds++;
        }
    }

    shader->setUniformUnsignedInt("u_materialFlags", materialFlags);
}

} // namespace siren::core

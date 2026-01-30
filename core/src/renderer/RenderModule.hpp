#pragma once

#include "resources/Buffer.hpp"
#include "PBRMaterial.hpp"
#include "resources/FrameBuffer.hpp"
#include "resources/GraphicsPipeline.hpp"
#include "RenderInfo.hpp"

#include "geometry/Mesh.hpp"
#include "geometry/Primitive.hpp"

#include "renderer/shaders/Shader.hpp"

#include "shaders/ShaderLibrary.hpp"


namespace siren::core
{
/**
 * @brief Render statistics for a single frame.
 */
struct RenderStats {
    u32 drawCalls     = 0;
    u32 vertices      = 0;
    u32 pipelineBinds = 0;
    u32 textureBinds  = 0;

    void Reset() {
        drawCalls     = 0;
        vertices      = 0;
        pipelineBinds = 0;
        textureBinds  = 0;
    }
};

struct alignas(16) LightUBO {
    Array<GPUPointLight, MAX_LIGHT_COUNT> pointLights;
    Array<GPUDirectionalLight, MAX_LIGHT_COUNT> directionalLights;
    Array<GPUSpotLight, MAX_LIGHT_COUNT> spotLights;
    u32 pointLightCount;
    u32 directionalLightCount;
    u32 spotLightCount;
    u32 _pad;
};

// static_assert(sizeof(LightUBO) == 16 * 32 * 3 + 4 * 4);

struct alignas(16) CameraUBO {
    glm::mat4 projectionView;
    glm::vec3 cameraPosition;
    float _pad;
};

static_assert(sizeof(CameraUBO) == 4 * 16 + 4 * 3 + 4);

/**
 * @brief The RenderModule is the 3D renderer of Siren. Responsible for submitting draw calls and managing render state.
 * @todo Make the RenderModule API agnostic!
 * @todo Make a static class? namespaced functions?
 */
class Renderer {
public:
    bool Init();
    void Shutdown();

    /// @brief Starts a new frame with the given @ref RenderInfo.
    void BeginFrame(const RenderInfo& renderInfo);
    /// @brief Ends the current frame.
    void EndFrame();

    /// @brief Begins a render pass. Optionally takes a @ref FrameBuffer and a clear color.
    void BeginPass(
        const Ref<FrameBuffer>& frameBuffer,
        const glm::vec4& clearColor = { 0, 0, 0, 1 }
    );
    /// @brief Ends a render pass. (GPU talk happens here)
    void EndPass();

    /// @brief Submits a mesh.
    void SubmitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform);

    /// @brief Return a reference to the current @ref RenderStats.
    const RenderStats& GetStats() const;
    /// @brief Returns the PBR pipeline.
    Ref<GraphicsPipeline> GetPBRPipeline() const;
    /// @brief Reloads all core shaders.
    void ReloadShaders();

private:
    void BindMaterial(const PBRMaterial* material, const Shader* shader);
    void DrawSkyLight();

    struct // container for pipelines
    {
        Ref<GraphicsPipeline> pbr;
        Ref<GraphicsPipeline> skybox;
        // Ref<GraphicsPipeline> wireframe;
        // Ref<GraphicsPipeline> unlit;
    } m_pipelines;

    Ref<PrimitiveMeshData> m_unitCube;

    RenderStats m_stats{ };
    RenderInfo m_renderInfo{ };

    ShaderLibrary m_shaderLibrary;

    FrameBuffer* m_currentFramebuffer = nullptr;

    Own<Buffer> m_cameraBuffer = nullptr; //< Bound to slot 0 always
    Own<Buffer> m_lightBuffer  = nullptr; //< Bound to slot 1 always

    /// @brief Struct containing a single draw command. Used for batching draw calls at the end of a frame.
    struct DrawCommand {
        u32 transformIndex;
        u32 indexCount;
        Buffer* vertices;
        Buffer* indices;
        GraphicsPipeline* pipeline;
        PBRMaterial* material;

        explicit operator bool() const { return indexCount > 0 && vertices && indices && pipeline && material; }
    };

    Vector<DrawCommand> m_drawQueue{ };
    Vector<glm::mat4> m_transforms{ };
};
} // namespace siren::core

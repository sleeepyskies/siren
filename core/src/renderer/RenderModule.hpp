#pragma once

#include "buffer/UniformBuffer.hpp"
#include "buffer/VertexArray.hpp"
#include "renderer/material/Material.hpp"
#include "FrameBuffer.hpp"
#include "renderConfig.hpp"
#include "core/Module.hpp"


namespace siren::core
{

/**
 * @brief Render statistics for a single frame.
 */
struct RenderStats
{
    u32 drawCalls    = 0;
    u32 triangles    = 0;
    u32 vertices     = 0;
    u32 textureBinds = 0;
    u32 shaderBinds  = 0;

    void reset()
    {
        drawCalls    = 0;
        triangles    = 0;
        vertices     = 0;
        textureBinds = 0;
        shaderBinds  = 0;
    }
};

/**
 * @brief The RenderModule is the 3D renderer of Siren. Responsible for submitting draw calls and managing render state.
 * @todo Make the RenderModule API agnostic!
 */
class RenderModule final : public Module
{
public:
    bool initialize() override;
    void shutdown() override;

    const char* getName() override { return "RenderModule"; }

    /// @brief Starts a new frame with the given @ref RenderInfo.
    void beginFrame(const RenderInfo& renderInfo);
    /// @brief Ends the current frame and presents it to the screen.
    void endFrame();

    /// @brief Begins a render pass. Optionally takes a @ref FrameBuffer and a clear color.
    void beginPass(
        const Ref<FrameBuffer>& frameBuffer = nullptr,
        const glm::vec4& clearColor         = { 0, 0, 0, 1 }
    );

    /// @brief Submit a mesh for drawing. Equates to a single draw call.
    void submit(
        const Ref<VertexArray>& vertexArray,
        const Ref<Material>& material,
        const glm::mat4& objectTransform
    );

    /// @brief Return a read only reference to the current @ref RenderStats.
    const RenderStats& getStats() const;

private:
    RenderInfo m_renderInfo{ };
    RenderStats m_stats{ };

    Ref<FrameBuffer> m_currentFramebuffer = nullptr;

    /// @brief Buffer holding universal camera data. Bound to slot 0
    Own<UniformBuffer> m_cameraBuffer = nullptr;
    /// @brief Buffer holding universal light data. Bound to slot 1
    Own<UniformBuffer> m_lightBuffer = nullptr;

    void setupLights();
    void setupCamera();
    void bindMaterial(const Ref<Material>& material);

    /**
     * @brief Struct containing a single draw command. Used for batching draw calls at the end of a frame.
     */
    struct DrawCommand
    {
        Ref<FrameBuffer> target;
        Ref<VertexArray> vertexArray;
        Ref<Material> material;
        glm::mat4 modelTransform;
    };

    std::vector<DrawCommand> m_drawQueue{ };
};

} // namespace siren::core

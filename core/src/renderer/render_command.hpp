#pragma once

#include "core/spch.hpp"

#include "resources/buffer.hpp"
#include "resources/framebuffer.hpp"
#include "resources/graphics_pipeline.hpp"


namespace siren::core
{

enum class RenderCommandType: u8 {
    BindGraphicsPipeline,
    SetViewport,
    BindVertexBuffer,
    BindIndexBuffer,
    DrawArrays,
    DrawIndexed,
    DrawInstanced,
};

struct BindGraphicsPipeline {
    GraphicsPipelineHandle pipeline_handle;
};

struct SetViewport {
    u32 x;
    u32 y;
    u32 width;
    u32 height;
};

struct BindVertexBuffer {
    BufferHandle vertex_buffer;
    u32 slot;
};

struct BindIndexBuffer {
    BufferHandle index_buffer;
    IndexFormat index_format;
};

struct DrawArrays {
    u32 start;
    u32 count;
};

struct DrawIndexed {
    u32 index_count;
    u32 first_index;
};

struct RenderCommand {
    union {
        BindGraphicsPipeline bind_graphics_pipeline;
        SetViewport set_viewport;
        BindVertexBuffer bind_vertex_buffer;
        BindIndexBuffer bind_index_buffer;
        DrawArrays draw_arrays;
        DrawIndexed draw_indexed;
    } command;

    RenderCommandType type;

    /// @brief Attempts to cast the internal command into a Command type. Crashes on fail.
    template <typename Command>
    auto as() const -> const Command& {
        if constexpr (std::is_same_v<Command, BindGraphicsPipeline>) {
            return command.bind_graphics_pipeline;
        } else if constexpr (std::is_same_v<Command, SetViewport>) {
            return command.set_viewport;
        } else if constexpr (std::is_same_v<Command, BindVertexBuffer>) {
            return command.bind_vertex_buffer;
        } else if constexpr (std::is_same_v<Command, BindIndexBuffer>) {
            return command.bind_index_buffer;
        } else if constexpr (std::is_same_v<Command, DrawArrays>) {
            return command.draw_arrays;
        } else if constexpr (std::is_same_v<Command, DrawIndexed>) {
            return command.draw_indexed;
        }
        static_assert(sizeof(Command) == -1, "Invalid Render Command type");
        SIREN_ASSERT(false, "Invalid Render Command. Cannot cast correctly");
    }
};

struct RenderPass {
    usize start;
    usize count;
};

/**
 * @brief Struct used to initialize and begin a new render pass.
 */
struct RenderPassDescriptor {
    /// @brief An optional label.
    /// @todo Not used anywhere atm
    std::optional<std::string> label;
    /// @brief The target to draw to.
    FramebufferHandle target;
};

class RenderPassRecorder {
public:
    RenderPassRecorder();
    ~RenderPassRecorder() = default;

    /**
     * @brief Binds a @ref GraphicsPipeline to the current render pass.
     * This sets up the state for any following draw commands to this recorder,
     * including shaders used, vertex layout, blend mode etc...
     * @param pipeline_handle The @ref GraphicsPipeline to bind.
     */
    auto bind_graphics_pipeline(GraphicsPipelineHandle pipeline_handle) noexcept -> void;

    /**
     * @brief Sets the viewport area for all following draw calls.
     * The viewport defines a transformation from NDC to the pixel coords
     * of the render target.
     * If not called, siren defaults to the full render target dimensions.
     * @param x The offset in pixels from the left side of the viewport.
     * @param y The offset in pixels from the top side of the viewport.
     * @param width The width in pixels of the viewport.
     * @param height The height in pixels of the viewport.
     */
    auto set_viewport(u32 x, u32 y, u32 width, u32 height) noexcept -> void;

    /**
     * @brief Assigns a vertex buffer to a slot.
     * Any following draw calls will use the provided buffer.
     * @note The caller should make sure the @ref Buffer layout matches the
     * layout in the bound @ref GraphicsPipeline.
     * @param slot The slot to bind to.
     * @param vertex_buffer The @ref Buffer to bind to the slot.
     */
    auto bind_vertex_buffer(u32 slot, BufferHandle vertex_buffer) noexcept -> void;

    /**
     * @brief Binds an index buffer to the current pass.
     * @note There may only be a single index buffer bound at a time.
     * @param index_buffer The index buffer to bind.
     * @param index_format The format of the indices (e.g., u8, u16, u32).
     */
    auto bind_index_buffer(BufferHandle index_buffer, IndexFormat index_format) noexcept -> void;

    /**
     * @brief Draws from the currently bound vertex buffer(s) non indexed.
     * @param start The first vertex to draw.
     * @param count The amount of vertices starting from the first to draw.
     */
    auto draw_arrays(u32 start, u32 count) noexcept -> void;

    /**
     * @brief Draws from the currently bound vertex buffer(s) using the currently
     * bound index buffer.
     * @param index_count The amount of indices to draw.
     * @param first_index The offset (in indices) into the index buffer to start from.
     */
    auto draw_indexed(u32 index_count, u32 first_index) noexcept -> void;

    /// @brief Consumes the RenderPassRecorder. Result should be passed into @ref RenderCommandRecorder.
    auto finish() -> std::vector<RenderCommand>&&;

private:
    /// @brief The accumulated commands.
    std::vector<RenderCommand> m_commands;
    /// @brief The bound pipeline.
    GraphicsPipelineHandle m_active_pipeline = GraphicsPipelineHandle::invalid();
    /// @brief The tracked vertex buffers.
    /// @todo replace with an array?
    std::flat_map<u32, BufferHandle> m_active_vertex_buffers;
    /// @brief The bound index buffer (we need to check index type too hence the struct).
    std::optional<BindIndexBuffer> m_active_index_buffer;
};

struct RenderCommandBuffer {
    /// @brief All recorded commands.
    std::vector<RenderCommand> m_commands;
    /// @brief Descriptions of which commands belong to which pass.
    std::vector<RenderPass> m_render_passes;
};

class RenderCommandRecorder {
public:
    RenderCommandRecorder()  = default;
    ~RenderCommandRecorder() = default;

    /// @brief Begins a xxx pass.
    [[nodiscard]] auto begin_render_pass() const noexcept -> RenderPassRecorder;
    /// @brief Consumes the result of a @ref RenderPassRecorder.
    auto consume_render_pass(std::vector<RenderCommand>&& commands) noexcept -> void;

    /// @brief Consumes the internal data of the RenderCommandBuffer ready for execution.
    [[nodiscard]] auto finish() noexcept -> RenderCommandBuffer;

private:
    /// @brief All recorded commands.
    std::vector<RenderCommand> m_commands;
    /// @brief Descriptions of which commands belong to which pass.
    std::vector<RenderPass> m_render_passes;

};

} // namespace siren::core

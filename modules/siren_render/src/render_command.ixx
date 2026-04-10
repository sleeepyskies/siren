module;

#include <vector>
#include <string>
#include <flat_map>
#include <optional>
#include <libassert/assert.hpp>

export module siren.render:render_command;

import :graphics_pipeline;
import :framebuffer;
import :buffer;

import siren.math;
import siren.common;

namespace siren::render {

// todo: optimization here to use a packed blob vector. we serialize the commands basically,
// and make use of a CommandHeader indicating the size and type to interpret the next bytes as

/**
 * @brief Identifies the type of operation recorded into the buffer.
 * Acts as a tag for a union.
 */
export enum class RenderCommandType: u8 {
    BindGraphicsPipeline,
    SetViewport,

    BindVertexBuffer,
    BindIndexBuffer,
    BindUniformBuffer,

    DrawArrays,
    DrawIndexed,
};

/**
 * @brief Indicates a @ref GraphicsPipeline bind. Sets all of its state.
 */
export struct BindGraphicsPipeline {
    /** @brief The pipeline to bind. */
    GraphicsPipeline::Handle pipeline_handle;
};

/**
 * @brief Sets the viewport transform (from NDC to target coords).
 */
export struct SetViewport {
    /** @brief The offset in pixels from the left of the viewport. */
    u32 x;
    /** @brief The offset in pixels from the top of the viewport. */
    u32 y;
    /** @brief The width in pixels of the viewport. */
    u32 width;
    /** @brief The height in pixels of the viewport. */
    u32 height;
};

/**
 * @brief Binds a vertex buffer to a slot.
 */
export struct BindVertexBuffer {
    /** @brief The buffer to bind. */
    Buffer::Handle vertex_buffer;
    /** @brief The slot to bind to. */
    u32 slot;
    /** @brief The offset into the vertex buffer. */
    u32 offset;
};

/**
 * @brief Binds an index buffer.
 */
export struct BindIndexBuffer {
    /** @brief The buffer to bind. */
    Buffer::Handle index_buffer;
    /** @brief The format of the indices. */
    IndexFormat index_format;
};

/**
 * @brief Binds a uniform buffer.
 */
export struct BindUniformBuffer {
    /** @brief The buffer to bind. */
    Buffer::Handle uniform_buffer;
    /** @brief The slot to bind to. */
    u32 slot;
};

/**
 * @brief Performs a non indexed draw call.
 */
export struct DrawArrays {
    /** @brief The start vertex to draw. */
    u32 start;
    /** @brief The amount of vertices to draw. */
    u32 count;
};

/**
 * @brief Performs an indexed draw call.
 */
export struct DrawIndexed {
    /** @brief The start index. */
    u32 first_index;
    /** @brief The number of indices to use. */
    u32 index_count;
};

/**
 * @brief Encapsulates a render related command.
 */
export struct RenderCommand {
    union {
        BindGraphicsPipeline bind_graphics_pipeline;
        SetViewport set_viewport;
        BindVertexBuffer bind_vertex_buffer;
        BindIndexBuffer bind_index_buffer;
        BindUniformBuffer bind_uniform_buffer;
        DrawArrays draw_arrays;
        DrawIndexed draw_indexed;
    } command;

    RenderCommandType type;

    /** @brief Attempts to cast the internal command into a Command type. Crashes on fail. */
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
        } else if constexpr (std::is_same_v<Command, BindUniformBuffer>) {
            return command.bind_uniform_buffer;
        } else if constexpr (std::is_same_v<Command, DrawArrays>) {
            return command.draw_arrays;
        } else if constexpr (std::is_same_v<Command, DrawIndexed>) {
            return command.draw_indexed;
        } else {
            static_assert(false, "Invalid Render Command type");
            UNREACHABLE("Invalid Render Command. Cannot cast correctly");
        }
    }
};

export enum class BeginOperation : u8 {
    /** @brief Clears the target a single color. */
    Clear,
    /** @brief Does nothing. */
    Fuckit,
};

/**
 * @brief Struct used to initialize and begin a new render pass.
 */
export struct RenderPassDescriptor {
    /** @brief An optional label. @todo Not used anywhere atm */
    std::optional<std::string> label;
    /** @brief The target to draw to. */
    FramebufferHandle target;
    /** @brief The action to perform on begin. */
    BeginOperation begin_operation;
    /** @brief The color to clear the target with on load iff begin_operation == Clear. Defaults to black. */
    std::optional<glm::vec4> clear_color;
};

/**
 * @brief Metadata about a render pass. Describes the range of
 * commands within a command buffer.
 */
export struct RenderPass {
    /** @brief The descriptor of the pass. */
    RenderPassDescriptor descriptor;
    /** @brief The start command index. */
    usize start;
    /** @brief The number of commands. */
    usize count;
};

export class RenderPassRecorder {
public:
    RenderPassRecorder();
    ~RenderPassRecorder() = default;

    /**
     * @brief Binds a @ref GraphicsPipeline to the current render pass.
     * This sets up the state for any following draw commands to this recorder,
     * including shaders used, vertex layout, blend mode etc...
     * @param pipeline_handle The @ref GraphicsPipeline to bind.
     */
    auto bind_graphics_pipeline(GraphicsPipeline::Handle pipeline_handle) noexcept -> void;

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
     * @param vertex_buffer The @ref Buffer to bind to the slot.
     * @param slot The slot to bind to.
     */
    auto bind_vertex_buffer(Buffer::Handle vertex_buffer, u32 slot) noexcept -> void;

    /**
     * @brief Binds an index buffer to the current pass.
     * @note There may only be a single index buffer bound at a time.
     * @param index_buffer The index buffer to bind.
     * @param index_format The format of the indices (e.g., u8, u16, u32).
     */
    auto bind_index_buffer(Buffer::Handle index_buffer, IndexFormat index_format) noexcept -> void;

    /**
     * @brief Binds a Uniform Buffer to the given slot.
     * @param uniform_buffer The @ref Buffer to bind to the slot.
     * @param slot The slot to bind to.
     */
    auto bind_uniform_buffer(Buffer::Handle uniform_buffer, u32 slot) noexcept -> void;

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

    /** @brief Consumes the RenderPassRecorder. Result should be passed into @ref RenderCommandRecorder. */
    auto finish() -> std::vector<RenderCommand>&&;

private:
    /** @brief The accumulated commands. */
    std::vector<RenderCommand> m_commands;
    /** @brief The bound pipeline. */
    GraphicsPipelineHandle m_active_pipeline = NullID;
    /** @brief The tracked vertex buffers. @todo replace with an array? */
    std::flat_map<u32, Buffer::Handle> m_active_vertex_buffers;
    /** @brief The tracked uniform buffers. @todo replace with an array? */
    std::flat_map<u32, Buffer::Handle> m_active_uniform_buffers;
    /** @brief The bound index buffer (we need to check index type too hence the struct). */
    std::optional<BindIndexBuffer> m_active_index_buffer;
};

export struct RenderCommandBuffer {
    /** @brief All recorded commands. */
    std::vector<RenderCommand> commands;
    /** @brief Descriptions of which commands belong to which pass. */
    std::vector<RenderPass> render_passes;
};

export class RenderCommandRecorder {
public:
    RenderCommandRecorder()  = default;
    ~RenderCommandRecorder() = default;

    /** @brief Begins a render pass. */
    [[nodiscard]] auto begin_render_pass() const noexcept -> RenderPassRecorder;
    /** @brief Consumes the result of a @ref RenderPassRecorder. */
    auto consume_render_pass(std::vector<RenderCommand>&& commands) noexcept -> void;

    /** @brief Consumes the internal data of the RenderCommandBuffer ready for execution. */
    [[nodiscard]] auto finish() noexcept -> RenderCommandBuffer;

private:
    /** @brief All recorded commands. */
    std::vector<RenderCommand> m_commands;
    /** @brief Descriptions of which commands belong to which pass. */
    std::vector<RenderPass> m_render_passes;

};

RenderPassRecorder::RenderPassRecorder() {
    // todo: no clue how much is good to reserve here
    m_commands.reserve(1024);
}

auto RenderPassRecorder::bind_graphics_pipeline(const GraphicsPipeline::Handle pipeline_handle) noexcept -> void {
    if (pipeline_handle == m_active_pipeline) { return; }

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .bind_graphics_pipeline = {
                    .pipeline_handle = pipeline_handle
                }
            },
            .type = RenderCommandType::BindGraphicsPipeline
        }
    );

    m_active_pipeline = pipeline_handle;
}

auto RenderPassRecorder::set_viewport(
    const u32 x,
    const u32 y,
    const u32 width,
    const u32 height
) noexcept -> void {
    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .set_viewport = {
                    .x = x,
                    .y = y,
                    .width = width,
                    .height = height,
                }
            },
            .type = RenderCommandType::SetViewport
        }
    );
}

auto RenderPassRecorder::bind_vertex_buffer(
    const Buffer::Handle vertex_buffer,
    const u32 slot
) noexcept -> void {
    const auto& it = m_active_vertex_buffers.find(slot);
    if (it != m_active_vertex_buffers.end() && it->second == vertex_buffer) { return; }

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .bind_vertex_buffer = {
                    .vertex_buffer = vertex_buffer,
                    .slot = slot,
                },
            },
            .type = RenderCommandType::BindVertexBuffer,
        }
    );

    m_active_vertex_buffers[slot] = vertex_buffer;
}

auto RenderPassRecorder::bind_index_buffer(
    const Buffer::Handle index_buffer,
    const IndexFormat index_format
) noexcept -> void {
    if (m_active_index_buffer.has_value()) {
        const auto& active = m_active_index_buffer.value();
        if (active.index_buffer == index_buffer && active.index_format == index_format) { return; }
    }

    const BindIndexBuffer cmd{ .index_buffer = index_buffer, .index_format = index_format };

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .bind_index_buffer = cmd,
            },
            .type = RenderCommandType::BindIndexBuffer
        }
    );

    m_active_index_buffer = cmd;
}

auto RenderPassRecorder::bind_uniform_buffer(
    const Buffer::Handle uniform_buffer,
    const u32 slot
) noexcept -> void {
    const auto& it = m_active_uniform_buffers.find(slot);
    if (it != m_active_vertex_buffers.end() && it->second == uniform_buffer) { return; }

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .bind_uniform_buffer = {
                    .uniform_buffer = uniform_buffer,
                    .slot = slot,
                },
            },
            .type = RenderCommandType::BindUniformBuffer,
        }
    );

    m_active_uniform_buffers[slot] = uniform_buffer;
}

auto RenderPassRecorder::draw_arrays(
    const u32 start,
    const u32 count
) noexcept -> void {
    ASSERT(
        m_active_pipeline.is_valid(),
        "There is no pipeline bound, cannot call RenderPassRecorder::draw_arrays."
    );

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .draw_arrays = {
                    .start = start,
                    .count = count
                }
            },
            .type = RenderCommandType::DrawArrays
        }
    );
}

auto RenderPassRecorder::draw_indexed(
    const u32 index_count,
    const u32 first_index
) noexcept -> void {
    ASSERT(
        m_active_pipeline.is_valid(),
        "There is no pipeline bound, cannot call RenderPassRecorder::draw_indexed."
    );
    ASSERT(
        m_active_index_buffer.has_value() && m_active_index_buffer.value().index_buffer.is_valid(),
        "There is no index buffer bound, cannot call RenderPassRecorder::draw_indexed."
    );
    ASSERT(
        m_active_vertex_buffers.size() > 0,
        "There are no vertex buffers bound, cannot call RenderPassRecorder::draw_indexed."
    );

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .draw_indexed = {
                    .first_index = first_index,
                    .index_count = index_count,
                }
            },
            .type = RenderCommandType::DrawIndexed,
        }
    );
}

auto RenderPassRecorder::finish() -> std::vector<RenderCommand>&& { return std::move(m_commands); }

// ============================================================================
// == MARK: RenderCommandRecorder
// ============================================================================

auto RenderCommandRecorder::begin_render_pass() const noexcept -> RenderPassRecorder {
    return RenderPassRecorder{ };
}

auto RenderCommandRecorder::consume_render_pass(std::vector<RenderCommand>&& commands) noexcept -> void {
    m_render_passes.emplace_back(
        RenderPass{
            .start = m_commands.size(),
            .count = commands.size()
        }
    );

    m_commands.insert(
        m_commands.end(),
        std::make_move_iterator(commands.begin()),
        std::make_move_iterator(commands.end())
    );
}

auto RenderCommandRecorder::finish() noexcept -> RenderCommandBuffer {
    return RenderCommandBuffer{ .commands = std::move(m_commands), .render_passes = std::move(m_render_passes) };
}

} // namespace siren::render

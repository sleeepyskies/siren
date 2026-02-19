#include "render_command.hpp"


namespace siren::core
{

// ============================================================================
// == MARK: RenderPassRecorder
// ============================================================================

RenderPassRecorder::RenderPassRecorder() {
    // todo: no clue how much is good to reserve here
    m_commands.reserve(1024);
}

auto RenderPassRecorder::bind_graphics_pipeline(const GraphicsPipelineHandle pipeline_handle) noexcept -> void {
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

auto RenderPassRecorder::set_viewport(const u32 x, const u32 y, const u32 width, const u32 height) noexcept -> void {
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

auto RenderPassRecorder::bind_vertex_buffer(const u32 slot, const BufferHandle vertex_buffer) noexcept -> void {
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
    const BufferHandle index_buffer,
    const IndexFormat index_format
) noexcept -> void {
    if (m_active_index_buffer.has_value()) {
        if (m_active_index_buffer.value().index_buffer == index_buffer && m_active_index_buffer.value().index_format ==
            index_format) { return; }
    }

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .bind_index_buffer = {
                    .index_buffer = index_buffer,
                    .index_format = index_format,
                },
            },
            .type = RenderCommandType::BindIndexBuffer
        }
    );

    m_active_index_buffer = index_buffer;
}

auto RenderPassRecorder::draw_arrays(const u32 start, const u32 count) noexcept -> void {
    SIREN_ASSERT(
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

auto RenderPassRecorder::draw_indexed(const u32 index_count, const u32 first_index) noexcept -> void {
    SIREN_ASSERT(
        m_active_pipeline.is_valid(),
        "There is no pipeline bound, cannot call RenderPassRecorder::draw_indexed."
    );
    SIREN_ASSERT(
        m_active_index_buffer.has_value() && m_active_index_buffer.value().index_buffer.is_valid(),
        "There is no index buffer bound, cannot call RenderPassRecorder::draw_indexed."
    );
    SIREN_ASSERT(
        m_active_vertex_buffers.size() > 0,
        "There are no vertex buffers bound, cannot call RenderPassRecorder::draw_indexed."
    );

    m_commands.emplace_back(
        RenderCommand{
            .command = {
                .draw_indexed = {
                    .index_count = index_count,
                    .first_index = first_index,
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

} // namespace siren::core

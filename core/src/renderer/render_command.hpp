#pragma once

#include "core/spch.hpp"

#include "resources/framebuffer.hpp"
#include "resources/graphics_pipeline.hpp"


namespace siren::core
{

enum class RenderCommandType {
    BindGraphicsPipeline,
};

struct BindGraphicsPipeline {
    GraphicsPipelineHandle pipeline_handle;
};

struct RenderCommand {
    union {
        BindGraphicsPipeline bind_graphics_pipeline;
    } command;

    RenderCommandType type;

    /// @brief Attempts to cast the internal command into a Command type. Crashes on fail.
    template <typename Command>
    auto as() const -> Command {
        if constexpr (std::is_same_v<Command, BindGraphicsPipeline>) {
            return command.bind_graphics_pipeline;
        } else if constexpr (std::is_same_v<Command, YYY>) {
            return command.yyy;
        }
        SIREN_ASSERT(false, "Invalid Render Command. Cannot cast correctly");
    }
};

struct RenderPass { };

/**
 * @brief Struct used to initialize and begin a new render pass.
 */
struct RenderPassDescriptor {
    /// @brief The target to draw to.
    FramebufferHandle target;
};

struct RenderCommandBuffer { };

class RenderCommandRecorder {
public:
    RenderCommandRecorder()  = default;
    ~RenderCommandRecorder() = default;

    /// @brief Begins a xxx pass.
    [[nodiscard]] auto begin_render_pass() const noexcept -> RenderPassRecorder;
    /// @brief
    auto consume_render_pass(RenderPass&& pass) noexcept -> void;

    /// @brief Consumes the internal data of the RenderCommandBuffer ready for execution.
    [[nodiscard]] auto finish() noexcept -> RenderCommandBuffer;

private:
    std::vector<RenderPass> m_render_passes;

};

} // namespace siren::core

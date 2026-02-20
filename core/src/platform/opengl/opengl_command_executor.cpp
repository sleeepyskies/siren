#include "opengl_command_executor.hpp"

#include "sync/render_thread.hpp"
#include "gl.hpp"
#include "mappings.hpp"


namespace siren::platform
{

using namespace siren::core;

// ============================================================================
// == MARK: Utilities
// ============================================================================

static constexpr auto render_thread() -> RenderThread& { return Locator<RenderThread>::locate(); }

static constexpr auto get_buffer_slice(
    const std::vector<u8>& buffer,
    const u32 offset,
    const u32 size
) -> std::span<const u8> {
    return std::span(buffer.data() + offset, size);
}

static constexpr auto extract_cmds(
    const RenderPass& pass,
    const std::vector<RenderCommand>& commands
) -> std::span<const RenderCommand> {
    SIREN_ASSERT(pass.start < commands.size(), "RenderPass has an invalid start index.");
    SIREN_ASSERT(pass.start <= commands.size(), "RenderPass has more commands than available");
    return std::span(commands.data() + pass.start, pass.count);
}

// ============================================================================
// == MARK: Execution Loops
// ============================================================================

OpenGLCommandExecutor::OpenGLCommandExecutor(const OpenGLRenderResourceState& state) : m_state(state) { }

auto OpenGLCommandExecutor::execute_resource_commands(ResourceCommandBuffer&& resource_command_pacakge) -> void {
    render_thread().spawn(
        [cmds = std::move(resource_command_pacakge), this] {
            for (const auto& cmd : cmds.commands) {
                switch (cmd.type) {
                    case ResourceCommandType::UploadImage: {
                        const auto& params = cmd.as<UploadImage>();
                        execute_image_upload(
                            std::move(cmd.command.upload_image_command),
                            get_buffer_slice(cmds.blob, params.data_offset, params.data_size)
                        );
                        break;
                    }
                    case ResourceCommandType::UploadBuffer: {
                        const auto& params = cmd.as<UploadBuffer>();
                        execute_buffer_upload(
                            std::move(cmd.command.upload_buffer_command),
                            get_buffer_slice(cmds.blob, params.blob_offset, params.data_size)
                        );
                        break;
                    }
                    default: SIREN_ASSERT(false, "Invalid ResourceCommandType encountered");
                }
            }
        }
    );
}

auto OpenGLCommandExecutor::execute_render_commands(RenderCommandBuffer&& render_command_package) -> void {
    render_thread().spawn(
        [cmds = std::move(render_command_package), this] {
            for (const auto& pass : cmds.render_passes) {
                execute_pass(pass.descriptor, extract_cmds(pass, cmds.commands));
            }
        }
    );
}

// ============================================================================
// == MARK: Resource Commands
// ============================================================================

auto OpenGLCommandExecutor::execute_image_upload(
    const UploadImage& cmd,
    const std::span<const u8> data_slice
) const -> void {
    // just upload it all in one go, this should be fine even for cube maps
    const auto gl_handle = m_state.image_table.fetch(cmd.image_handle);
    const auto& desc     = m_state.image_table.extra(cmd.image_handle).descriptor;

    glTextureSubImage3D(
        gl_handle,
        0,
        0,
        0,
        0,
        desc.extent.width,
        desc.extent.height,
        desc.extent.depth_or_layers,
        gl::img_format_to_gl_layout(desc.format),
        GL_UNSIGNED_BYTE,
        data_slice.data()
    );

    // generate mip map levels
    if (desc.mipmap_levels > 0) {
        glGenerateTextureMipmap(gl_handle);
    }
}

auto OpenGLCommandExecutor::execute_buffer_upload(
    const UploadBuffer& cmd,
    const std::span<const u8> data_slice
) const -> void {
    const auto gl_handle = m_state.buffer_table.fetch(cmd.buffer_handle);
    const auto& desc     = m_state.buffer_table.extra(cmd.buffer_handle).descriptor;

    switch (desc.usage) {
        case BufferUsage::Static: {
            // create a temp staging buffer to copy data to the dest buffer
            GLuint staging_buffer;
            glCreateBuffers(1, &staging_buffer);
            glNamedBufferStorage(staging_buffer, data_slice.size(), data_slice.data(), 0);

            // perform transfer
            glCopyNamedBufferSubData(
                staging_buffer,
                gl_handle,
                0,
                cmd.dest_offset,
                data_slice.size()
            );

            // clean up staging buffer.
            glDeleteBuffers(1, &staging_buffer);
            break;
        }
        case BufferUsage::Dynamic: {
            glNamedBufferSubData(
                gl_handle,
                cmd.dest_offset,
                data_slice.size(),
                data_slice.data()
            );
            break;
        }
        case BufferUsage::Stream: {
            const auto mapped = m_state.buffer_table.extra(cmd.buffer_handle).buffer_ptr;
            SIREN_ASSERT(mapped.ptr != nullptr, "Stream Buffer mapped pointer is null!");
            SIREN_ASSERT(
                mapped.size - cmd.dest_offset >= data_slice.size(),
                "Attempted to overwrite a Streamed mapped buffer!"
            );
            std::memcpy(
                static_cast<u8*>(mapped.ptr) + cmd.dest_offset,
                data_slice.data(),
                data_slice.size()
            );
            break;
        }
        default: SIREN_ASSERT(
                false,
                "Invalid BufferUsage encountered. Cannot perform execute_buffer_upload on the OpenGL Backend"
            );
    }
}

// ============================================================================
// == MARK: Render Commands
// ============================================================================

auto OpenGLCommandExecutor::execute_pass(
    const RenderPassDescriptor& descriptor,
    const std::span<const RenderCommand> commands
) const -> void {
    const auto fb_handle      = m_state.framebuffer_table.fetch(descriptor.target);
    const auto& fb_descriptor = m_state.framebuffer_table.extra(descriptor.target).descriptor;

    // first setup pass
    if (descriptor.begin_operation == BeginOperation::Clear) {
        glm::vec4 color = descriptor.clear_color.value_or(RGBA::BLACK);

        // clear color attachments
        if (descriptor.clear_color.has_value()) {
            color = descriptor.clear_color.value();
        }
        for (const auto color_index : views::iota(0u, fb_descriptor.num_colors)) {
            glClearNamedFramebufferfv(fb_handle, GL_COLOR, color_index, &color.x);
        }

        // clear depth stencil
        if (fb_descriptor.has_depth_stencil) {
            glClearNamedFramebufferfi(fb_handle, GL_DEPTH_STENCIL, 0, 1.f, 0);
        }
    }

    // execute commands in the pass
    for (const auto& cmd : commands) {
        switch (cmd.type) {
            case RenderCommandType::BindGraphicsPipeline: {
                auto& bind = cmd.as<BindGraphicsPipeline>();
            }
            case RenderCommandType::SetViewport: break;
            case RenderCommandType::BindVertexBuffer: break;
            case RenderCommandType::BindIndexBuffer: break;
            case RenderCommandType::BindUniformBuffer: break;
            case RenderCommandType::DrawArrays: break;
            case RenderCommandType::DrawIndexed: break;
            case RenderCommandType::DrawInstanced: break;
        }
    }

    // clean up pass
}

auto OpenGLCommandExecutor::bind_graphics_pipeline(const BindGraphicsPipeline& bind) const -> void {
    auto& gp_table           = m_state.graphics_pipeline_table;
    const auto va_handle     = gp_table.fetch(bind.pipeline_handle);
    const auto shader_handle = gp_table.extra(bind.pipeline_handle).shader_program_handle;
    const auto& desc         = gp_table.extra(bind.pipeline_handle).descriptor;

    // bind the shader and vertex array == vertex layout
    glUseProgram(shader_handle);
    glBindVertexArray(va_handle);

    // set render state
    switch (desc.alpha_mode) {
        case AlphaMode::Opaque: {
            glEnable(GL_BLEND);
            break;
        }
        case AlphaMode::Blend: {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        case AlphaMode::Mask: {
            // shader has to handle discarding of fragments
            glEnable(GL_BLEND);
            break;
        }
    }

    glDepthFunc(gl::depth_func_to_gl(desc.depth_function));

    if (desc.back_face_culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }

    if (desc.depth_write) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }

    if (desc.depth_test) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    // draw mode aka PrimitiveTopology cannot be set here. Instead, we must
    // pass it in with each draw call.
}

} // namespace siren::platform

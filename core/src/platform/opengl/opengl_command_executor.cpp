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
    // TODO: implement
}

// ============================================================================
// == MARK: Single exec methods
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
    const UploadBuffer& cmd, const std::span<const u8> data_slice
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

} // namespace siren::platform

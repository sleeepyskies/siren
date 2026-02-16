#include "resource_command_buffer.hpp"


namespace siren::core
{

auto ResourceCommandBuffer::upload_to_buffer(const BufferHandle buffer_handle, std::span<const u8> data) -> void {
    const UploadBuffer upload{
        .buffer_handle = buffer_handle,
        .blob_offset = m_blob.size(),
        .data_size = data.size(),
    };

    m_blob.insert(m_blob.end(), data.begin(), data.end());

    m_commands.emplace_back(
        ResourceCommand{
            .command = {
                .upload_buffer_command = upload
            },
            .type = ResourceCommandType::UploadBuffer,
        }
    );
}

auto ResourceCommandBuffer::upload_to_image(const Image* image, std::span<const u8> data) -> void {
    if (image == nullptr) { return; }

    const UploadImage upload{
        .image_handle = image->handle(),
        .data_offset = m_blob.size(),
        .data_size = data.size(),
        .extent = image->descriptor().extent,
        .format = image->descriptor().format,
        .mipmap_levels = image->descriptor().mipmap_levels,
    };

    m_blob.insert(m_blob.end(), data.begin(), data.end());

    m_commands.emplace_back(
        ResourceCommand{
            .command = {
                .upload_image_command = upload
            },
            .type = ResourceCommandType::UploadImage,
        }
    );
}

auto ResourceCommandBuffer::finish() noexcept -> ResourceCommandPacakge {
    return ResourceCommandPacakge{ .commands = std::move(m_commands), .blob = std::move(m_blob) };
}

} // namespace siren::core

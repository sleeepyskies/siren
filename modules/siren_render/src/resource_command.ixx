module;

#include <concepts>
#include <span>
#include "assert.hpp"

export module siren.render.resource_command;

import siren.common;
import siren.render.buffer;
import siren.render.image;

namespace siren::render {

// todo: optimization here to use a packed blob vector. we serialize the commands basically,
// and make use of a CommandHeader indicating the size and type to interpret the next bytes as

/**
 * @brief Identifies the type of operation recorded into the buffer.
 * Acts as a tag for a union.
 */
export enum class ResourceCommandType : u8 {
    UploadBuffer,
    UploadImage,
};

/**
 * @brief Parameters for CPU to GPU data upload @ref Buffer copy.
 */
export struct UploadBuffer {
    /** @brief The handle of the target @ref Buffer. */
    Buffer::Handle buffer_handle;
    /** @brief The offset of the data to copy in the @ref ResourceCommandBuffer blob. */
    usize blob_offset;
    /** @brief The offset to write into the destination buffer. */
    usize dest_offset;
    /** @brief The size of the data to copy in the @ref ResourceCommandBuffer blob. */
    usize data_size;
};

/**
 * @brief Parameters for CPU to GPU data upload @ref Image copy.
 */
export struct UploadImage {
    /** @brief The handle of the target @ref Image. */
    Image::Handle image_handle;
    /** @brief The offset of the data to copy in the @ref ResourceCommandBuffer blob. */
    usize data_offset;
    /** @brief The size of the data to copy in the @ref ResourceCommandBuffer blob. */
    usize data_size;
};

/**
 * @brief Encapsulates a single resource command. Is essentially a tagged union.
 */
export struct ResourceCommand {
    union {
        UploadBuffer upload_buffer_command;
        UploadImage upload_image_command;
    } command;

    ResourceCommandType type;

    /** @brief Attempts to cast the internal command into a Command type. Crashes on fail. */
    template <typename Command>
    auto as() const -> Command {
        if constexpr (std::is_same_v<Command, UploadBuffer>) {
            return command.upload_buffer_command;
        } else if constexpr (std::is_same_v<Command, UploadImage>) {
            return command.upload_image_command;
        } else {
            static_assert(false, "Invalid Resource Command type");
            SIREN_ASSERT(false, "Invalid Resource Command. Cannot cast correctly");
        }
    }
};

/**
 * @brief Represents the result of recording commands
 * into the @ref ResourceCommandBuffer.
 */
export struct ResourceCommandBuffer {
    /** @brief The recorded commands. */
    std::vector<ResourceCommand> commands;
    /** @brief Raw storage for all upload tasks. */
    std::vector<u8> blob;
};

/**
 * @brief Records resource operations such as data uploads/copies.
 * Is not designed to be thread safe, as it is assumed each thread
 * receives a unique ResourceCommandBuffer to use.
 *
 * Should be passed to the @ref Device once completed.
 */
export class ResourceCommandRecorder {
public:
    ResourceCommandRecorder()  = default;
    ~ResourceCommandRecorder() = default;

    /** @brief Uploads the given CPU data to a @ref Buffer. */
    /** The caller must ensure the @ref Buffer is large enough. */
    auto upload_to_buffer(Buffer::Handle buffer_handle, std::span<const u8> data, u32 dest_offset) -> void;

    /** @brief Uploads the given data to an @ref Image. */
    auto upload_to_image(Image::Handle image_handle, std::span<const u8> data) -> void;

    /** @brief Consumes the internal data of the ResourceCommandBuffer ready for execution. */
    [[nodiscard]] auto finish() noexcept -> ResourceCommandBuffer;

private:
    friend class OpenGLCommandExecutor;

    /** @brief The recorded commands. */
    std::vector<ResourceCommand> m_commands;
    /** @brief Raw storage for all upload tasks. */
    std::vector<u8> m_blob;
};

auto ResourceCommandRecorder::upload_to_buffer(
    const Buffer::Handle buffer_handle,
    std::span<const u8> data,
    const u32 dest_offset
) -> void {
    const UploadBuffer upload{
        .buffer_handle = buffer_handle,
        .blob_offset = m_blob.size(),
        .dest_offset = dest_offset,
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

auto ResourceCommandRecorder::upload_to_image(
    const Image::Handle image_handle,
    std::span<const u8> data
) -> void {
    const UploadImage upload{
        .image_handle = image_handle,
        .data_offset = m_blob.size(),
        .data_size = data.size(),
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

auto ResourceCommandRecorder::finish() noexcept -> ResourceCommandBuffer {
    return ResourceCommandBuffer{ .commands = std::move(m_commands), .blob = std::move(m_blob) };
}

} // namespace siren::render

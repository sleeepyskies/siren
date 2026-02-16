#pragma once

#include "resources/buffer.hpp"
#include "resources/image.hpp"


namespace siren::core
{

/**
 * @brief Identifies the type of operation recorded into the buffer.
 * Acts as a tag for a union.
 */
enum class ResourceCommandType : u8 {
    /// @see siren::core::UploadBuffer.
    UploadBuffer,
    /// @see siren::core::UploadImage.
    UploadImage,
};

/**
 * @brief Parameters for CPU to GPU data upload @ref Buffer copy.
 */
struct UploadBuffer {
    /// @brief The handle of the target @ref Buffer.
    BufferHandle buffer_handle;
    /// @brief The offset of the data to copy in the @ref ResourceCommandBuffer blob.
    usize blob_offset;
    /// @brief The offset to write into the destination buffer.
    usize dest_offset;
    /// @brief The size of the data to copy in the @ref ResourceCommandBuffer blob.
    usize data_size;
    /// @brief The usage of the target @ref Buffer.
    BufferUsage buffer_usage;
};

/**
 * @brief Parameters for CPU to GPU data upload @ref Image copy.
 */
struct UploadImage {
    /// @brief The handle of the target @ref Image.
    ImageHandle image_handle;
    /// @brief The offset of the data to copy in the @ref ResourceCommandBuffer blob.
    usize data_offset;
    /// @brief The size of the data to copy in the @ref ResourceCommandBuffer blob.
    usize data_size;
    /// @brief The size of the image.
    ImageExtent extent;
    /// @brief The data format of the image.
    ImageFormat format;
    /// @brief The desired amount of mip map levels to generate.
    u32 mipmap_levels;
};

/**
 * @brief Encapsulates a single resource command. Is essentially a tagged union.
 */
struct ResourceCommand {
    union {
        UploadBuffer upload_buffer_command;
        UploadImage upload_image_command;
    } command;

    ResourceCommandType type;

    template <typename Command>
    auto as() const -> Command {
        if constexpr (std::is_same_v<Command, UploadBuffer>) {
            return command.upload_buffer_command;
        } else if constexpr (std::is_same_v<Command, UploadImage>) {
            return command.upload_image_command;
        }
        SIREN_ASSERT(false, "Invalid Resource Command. Cannot cast correctly");
    }
};

struct ResourceCommandPacakge {
    /// @brief The recorded commands.
    std::vector<ResourceCommand> commands;
    /// @brief Raw storage for all upload tasks.
    std::vector<u8> blob;
};

/**
 * @brief Records resource operations such as data uploads/copies.
 * Is not designed to be thread safe, as it is assumed each thread
 * receives a unique ResourceCommandBuffer to use.
 *
 * Should be passed to the @ref Device once completed.
 */
class ResourceCommandBuffer final {
public:
    ResourceCommandBuffer()  = default;
    ~ResourceCommandBuffer() = default;

    /// @brief Uploads the given CPU data to a @ref Buffer.
    /// The caller must ensure the @ref Buffer is large enough.
    auto upload_to_buffer(BufferHandle buffer_handle, std::span<const u8> data) -> void;
    /// @brief Uploads the given data to an @ref Image.
    auto upload_to_image(const Image* image, std::span<const u8> data) -> void;

    /// @brief Consumes the internal data of the ResourceCommandBuffer ready for execution.
    [[nodiscard]] auto finish() noexcept -> ResourceCommandPacakge;

private:
    friend class OpenGLCommandExecutor;

    /// @brief The recorded commands.
    std::vector<ResourceCommand> m_commands;
    /// @brief Raw storage for all upload tasks.
    std::vector<u8> m_blob;
};

} // namespace siren::core

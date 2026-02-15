#pragma once

namespace siren::core
{
// todo: can we optimise here using a render buffer object?

class Image;

class Framebuffer;
using FramebufferHandle = RenderResourceID<Framebuffer>;

/**
 * @brief Describes a @ref Framebuffer. Used for object creation via @ref Device.
 */
struct FramebufferDescriptor {
    /// @brief The optional label of the @ref Framebuffer. Mainly used for debugging.
    std::optional<std::string> label;
    /// @brief The width of the desired @ref Framebuffer in pixels.
    u32 width;
    /// @brief The height of the desired @ref Framebuffer in pixels.
    u32 height;
    u32 num_samples = 1; // TODO: not used for now :D
    bool has_color;
    bool has_depth;
    bool has_stencil;
};

class Framebuffer final : RenderResource<Framebuffer> {
    using Base = RenderResource<Framebuffer>;

public:
    explicit Framebuffer(
        Device* device,
        FramebufferHandle handle,
        const FramebufferDescriptor& descriptor
    );
    ~Framebuffer();

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer& other) noexcept;

    /// @brief Returns the @ref FramebufferDescriptor used to create this Framebuffer.
    [[nodiscard]] auto descriptor() const noexcept -> const FramebufferDescriptor&;

    void set_viewport() const;

    void resize(u32 width, u32 height);

private:
    /// @brief The construction parameters of the Framebuffer.
    FramebufferDescriptor m_descriptor;

    /// @brief The color attachment.
    std::unique_ptr<Image> m_color;
    /// @brief The depth attachment.
    std::unique_ptr<Image> m_depth;
    /// @brief The stencil attachment.
    std::unique_ptr<Image> m_stencil;

    void create();
};
} // namespace siren::core

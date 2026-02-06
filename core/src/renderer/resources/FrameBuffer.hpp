#pragma once

#include "Texture.hpp"


namespace siren::core
{
// todo: can we optimise here using a render buffer object?

class FrameBuffer;
using FrameBufferHandle = GpuResourceHandle<FrameBuffer>;

class FrameBuffer final : GpuResource {
public:
    struct Properties {
        u32 width, height;
        u32 numSamples        = 1; // TODO: not used for now :D
        bool hasColorBuffer   = true;
        bool hasDepthBuffer   = false;
        bool hasStencilBuffer = false;
    };

    explicit FrameBuffer(const Properties& properties);
    ~FrameBuffer() override;

    FrameBuffer(FrameBuffer&)            = delete;
    FrameBuffer& operator=(FrameBuffer&) = delete;

    const Properties& properties() const;

    void bind() const;
    void unbind() const;

    FrameBufferHandle handle() const;

    void set_viewport() const;

    std::optional<ImageHandle> color_attachment_handle() const;
    std::optional<ImageHandle> depth_attachment_handle() const;
    std::optional<ImageHandle> stencil_attachment_handle() const;

    void resize(u32 width, u32 height);

private:
    Properties m_properties;
    FrameBufferHandle m_handle;

    std::unique_ptr<Texture> m_color   = nullptr;
    std::unique_ptr<Texture> m_depth   = nullptr;
    std::unique_ptr<Texture> m_stencil = nullptr;

    void create();
};
} // namespace siren::core

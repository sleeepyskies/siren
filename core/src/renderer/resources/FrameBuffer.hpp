#pragma once

#include "Texture.hpp"


namespace siren::core
{
// todo: can we optimise here using a render buffer object?

class FrameBuffer;
using FrameBufferHandle = RenderResourceID<FrameBuffer>;

class FrameBuffer final : RenderResource {
public:
    struct Description {
        u32 width, height;
        u32 num_samples         = 1; // TODO: not used for now :D
        bool has_color_buffer   = true;
        bool has_depth_buffer   = false;
        bool has_stencil_buffer = false;
    };

    explicit FrameBuffer(const Description& description);
    ~FrameBuffer() override;

    FrameBuffer(FrameBuffer&)            = delete;
    FrameBuffer& operator=(FrameBuffer&) = delete;

    const Description& description() const;

    void bind() const;
    void unbind() const;

    FrameBufferHandle handle() const;

    void set_viewport() const;

    Texture* color_attachment() const;
    Texture* depth_attachment() const;
    Texture* stencil_attachment() const;

    void resize(u32 width, u32 height);

private:
    Description m_description;
    FrameBufferHandle m_handle;

    std::unique_ptr<Texture> m_color   = nullptr;
    std::unique_ptr<Texture> m_depth   = nullptr;
    std::unique_ptr<Texture> m_stencil = nullptr;

    void create();
};
} // namespace siren::core

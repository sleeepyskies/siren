#pragma once

#include "image.hpp"


namespace siren::core
{
// todo: can we optimise here using a render buffer object?

class Framebuffer;
using FramebufferHandle = RenderResourceID<Framebuffer>;

struct FramebufferDescriptor {
    u32 width, height;
    u32 num_samples         = 1; // TODO: not used for now :D
    bool has_color_buffer   = true;
    bool has_depth_buffer   = false;
    bool has_stencil_buffer = false;
};

class Framebuffer final : RenderResource<Framebuffer> {
public:
    struct Description { };

    explicit Framebuffer(const Description& description);
    ~Framebuffer();

    Framebuffer(Framebuffer&)            = delete;
    Framebuffer& operator=(Framebuffer&) = delete;

    const Description& description() const;

    void bind() const;
    void unbind() const;

    FramebufferHandle handle() const;

    void set_viewport() const;

    Texture* color_attachment() const;
    Texture* depth_attachment() const;
    Texture* stencil_attachment() const;

    void resize(u32 width, u32 height);

private:
    Description m_description;
    FramebufferHandle m_handle;

    std::unique_ptr<Texture> m_color   = nullptr;
    std::unique_ptr<Texture> m_depth   = nullptr;
    std::unique_ptr<Texture> m_stencil = nullptr;

    void create();
};
} // namespace siren::core

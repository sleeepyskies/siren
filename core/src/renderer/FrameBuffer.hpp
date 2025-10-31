#pragma once

#include "Texture.hpp"


namespace siren::core
{

// todo: can we optimise here using a render buffer object?

class FrameBuffer
{
public:
    struct Properties
    {
        u32 width, height;
        u32 numSamples        = 1; // TODO: not used for now :D
        bool hasColorBuffer   = true;
        bool hasDepthBuffer   = false;
        bool hasStencilBuffer = false;
    };

    explicit FrameBuffer(const Properties& properties);
    ~FrameBuffer();
    const Properties& getProperties() const;

    void bind() const;
    static void unbind();

    u32 getId() const;

    void setViewport() const;

    Maybe<u32> getColorAttachmentId() const;
    Maybe<u32> getDepthAttachmentId() const;
    Maybe<u32> getStencilAttachmentId() const;

    void resize(u32 width, u32 height);

private:
    Properties m_properties;
    u32 m_id;

    Own<Texture2D> m_color   = nullptr;
    Own<Texture2D> m_depth   = nullptr;
    Own<Texture2D> m_stencil = nullptr;

    void create();
};

} // namespace siren::core

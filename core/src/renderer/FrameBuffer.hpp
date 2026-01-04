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

    FrameBuffer(FrameBuffer&)            = delete;
    FrameBuffer& operator=(FrameBuffer&) = delete;

    const Properties& getProperties() const;

    void Bind() const;
    void Unbind() const;

    u32 GetID() const;

    void SetViewport() const;

    Maybe<u32> GetColorAttachmentID() const;
    Maybe<u32> GetDepthAttachmentID() const;
    Maybe<u32> GetStencilAttachmentID() const;

    void Resize(u32 width, u32 height);

private:
    Properties m_properties;
    u32 m_id;

    Own<Texture2D> m_color   = nullptr;
    Own<Texture2D> m_depth   = nullptr;
    Own<Texture2D> m_stencil = nullptr;

    void Create();
};
} // namespace siren::core

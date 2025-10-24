#pragma once

#include "Texture.hpp"

namespace siren::core
{

// todo: can we optimise here using a render buffer object?

class FrameBuffer
{
public:
    struct Properties {
        uint32_t width, height;
        uint32_t numSamples   = 1; // TODO: not used for now :D
        bool hasColorBuffer   = true;
        bool hasDepthBuffer   = false;
        bool hasStencilBuffer = false;
        glm::vec4 clearColor{0};
    };

    explicit FrameBuffer(const Properties& properties);
    ~FrameBuffer();
    const Properties& getProperties() const;

    void bind() const;
    void unbind() const;

    void prepare() const;
    void setClearColor(glm::vec4 color);
    void clearBuffers() const;
    void setViewport() const;

    Maybe<uint32_t> getColorAttachmentId() const;
    Maybe<uint32_t> getDepthAttachmentId() const;
    Maybe<uint32_t> getStencilAttachmentId() const;

    void resize(uint32_t width, uint32_t height);

private:
    Properties m_properties;
    uint32_t m_id;

    Uref<Texture2D> m_color   = nullptr;
    Uref<Texture2D> m_depth   = nullptr;
    Uref<Texture2D> m_stencil = nullptr;

    void create();
};

} // namespace siren::core

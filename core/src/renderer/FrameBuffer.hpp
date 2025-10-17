#pragma once

#include "Texture.hpp"

namespace siren::renderer
{

class FrameBuffer
{
public:
    struct Properties {
        uint32_t width, height;
        uint32_t numSamples   = 1; // TODO: not used for now :D
        bool hasColorBuffer   = true;
        bool hasDepthBuffer   = false;
        bool hasStencilBuffer = false;
    };

    explicit FrameBuffer(const Properties& properties);
    ~FrameBuffer();
    const Properties& getProperties() const;

    void bind() const;
    void unbind() const;

private:
    Properties m_properties;
    uint32_t m_id;

    Uref<Texture2D> m_color   = nullptr;
    Uref<Texture2D> m_depth   = nullptr;
    Uref<Texture2D> m_stencil = nullptr;
};

} // namespace siren::renderer

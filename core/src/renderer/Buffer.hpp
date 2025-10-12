#pragma once

#include "BufferCommon.hpp"
#include "core/GL.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{


class UniformBuffer
{
public:
    UniformBuffer();
    ~UniformBuffer();

    void bind() const;
    void unbind() const;
    void uploadData(const std::vector<Byte>& data, BufferUsage usage, uint8_t slot) const;
    BufferID id() const;

private:
    BufferID m_id = 0;
};

} // namespace siren::renderer

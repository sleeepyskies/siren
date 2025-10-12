#pragma once

#include "BufferCommon.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{

/**
 * @brief Represents a set of Uniform Variables on the GPU. Uniform variables are constants used
 * across all stages of the pipeline as well as across multiple shader invocations.
 */
class UniformBuffer
{
public:
    /// @brief Creates a uniform buffer object on the GPU and uploads the data to it
    UniformBuffer(const std::vector<Byte>& data, BufferUsage usage);
    ~UniformBuffer();

    /// @brief Binds this UniformBuffer, aka sets it as the active UniformBuffer for OpenGL
    void bind() const;
    /// @brief Unbinds whatever UniformBuffer is currently bound.
    void unbind() const;
    /// @brief Attaches this UniformBuffer to the given slot, making it accessible by shaders at
    /// that slot
    void attach(int location) const;

private:
    BufferID m_id = 0;
};

} // namespace siren::renderer

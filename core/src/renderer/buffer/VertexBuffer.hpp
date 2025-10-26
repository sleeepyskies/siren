/**
 * @file VertexBuffer.hpp
 */
#pragma once

#include "BufferCommon.hpp"
#include "VertexBufferLayout.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

struct VertexData {
    std::vector<glm::vec3> positions{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec3> tangents{};
    std::vector<glm::vec3> bitangents{};
    std::vector<glm::vec2> textureUvs{};
    std::vector<glm::vec4> colors{};
};

/**
 * @brief Vertex Buffer Object. Represents a raw block of GPU memory storing vertex
 * data. This may be positions, colors, normals, texture coordinates etc...
 */
class VertexBuffer
{
public:
    /// @brief Creates a CPU side object as well as uploads the data to the GPU
    VertexBuffer(const VertexData& data, BufferUsage usage);
    ~VertexBuffer();

    /// @brief Binds the VertexBuffer, aka sets it as the currently active Vertex Buffer for OpenGL
    void bind() const;
    /// @brief Unbinds whatever VertexBuffer is currently bound.
    void unbind() const;

    /// @brief Returns the current layout
    VertexBufferLayout getLayout() const;

private:
    BufferID m_id = 0;
    VertexBufferLayout m_layout{};
};

} // namespace siren::core

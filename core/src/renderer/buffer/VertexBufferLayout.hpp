#pragma once

#include "../../platform/GL.hpp"

namespace siren::core
{

/**
 * @brief Enum listing all allowed Shader Attributes.
 */
enum class VertexAttribute {
    POSITION,
    NORMAL,
    TANGENT,
    BITANGENT,
    TEXTUREUV,
    COLOR,
};

/**
 * @brief Describes how the GPU should read the data for one attribute from a Vertex Buffer.
 */
struct VertexBufferElement {
    /// @brief The name of this attribute
    VertexAttribute attribute{};
    /// @brief The index at which this vertex attribute is bound
    u32 index{0};
    /// @brief The number of components per vertex attribute
    i32 size{0};
    /// @brief The datatype of this vertex attribute
    GLenum type{GL_FLOAT}; // only use floats for now, no need to optimise yet
    /// @brief Whether the data is normalized
    bool normalized{false}; // hardcoded to false for now as I have no use for
    /// @brief The byte offset between vertex attributes
    size_t stride{0};
    /// @brief The byte offset of the first vertex attribute into the whole VBO
    size_t offset{0};

    explicit VertexBufferElement(const VertexAttribute attribute) : attribute(attribute) {}
};

/**
 * @brief Describes the full layout of a single Vertex Buffer. Note that the order in which elements
 * are added should match the underlying buffer. Adding elements updates all held elements in the
 * layout
 */
class VertexBufferLayout
{
public:
    /// @brief Update the layout with a new element
    void addVertexAttribute(const VertexAttribute& attribute);
    /// @brief Returns the layout
    std::vector<VertexBufferElement> getLayout() const;
    /// @brief Computes and returns the total stride of the vertices.
    u32 getStride() const;

private:
    std::vector<VertexBufferElement> m_elements{};
};

} // namespace siren::core

#pragma once

#include "../../platform/GL.hpp"

namespace siren::renderer
{

/**
 * @brief Enum listing all allowed Shader Attributes.
 */
enum class ShaderAttribute {
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
    ShaderAttribute attribute{};
    /// @brief The index at which this vertex attribute is bound
    uint32_t index{ 0 };
    /// @brief The number of components per vertex attribute
    int32_t size{ 0 };
    /// @brief The datatype of this vertex attribute
    GLenum type{ GL_FLOAT }; // only use floats for now, no need to optimise yet
    /// @brief Whether the data is normalized
    bool normalized{ false }; // hardcoded to false for now as I have no use for
    /// @brief The byte offset between vertex attributes
    size_t stride{ 0 };
    /// @brief The byte offset of the first vertex attribute into the whole VBO
    size_t offset{ 0 };

    explicit VertexBufferElement(const ShaderAttribute attribute) : attribute(attribute)
    {
    }
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
    void addVertexAttribute(const ShaderAttribute& attribute);
    /// @brief Returns the layout
    std::vector<VertexBufferElement> getLayout() const;

private:
    std::vector<VertexBufferElement> m_elements{};
};

} // namespace siren::renderer

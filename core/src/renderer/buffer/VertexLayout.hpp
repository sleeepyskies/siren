#pragma once

#include "platform/GL.hpp"


namespace siren::core
{
/**
 * @brief Enum listing all allowed Shader Attributes.
 */
enum class VertexAttribute
{
    Position,
    Normal,
    Tangent,
    Bitangent,
    Texture,
    Color,
};

/**
 * @brief Describes how the GPU should read the data for one attribute from a Vertex Buffer.
 */
struct VertexElement
{
    /// @brief The name of this attribute
    VertexAttribute attribute{ };
    /// @brief The number of components per vertex attribute
    u32 size{ 0 };
    /// @brief The datatype of this vertex attribute
    GLenum type{ GL_FLOAT }; // only use floats for now, no need to optimise yet
    /// @brief Whether the data is normalized
    bool normalized{ false }; // hardcoded to false for now as I have no use for
    /// @brief The byte offset of the first vertex attribute into the whole VBO
    size_t offset{ 0 };
};

/**
 * @brief Describes the full layout of a single Vertex Buffer. Note that the order in which elements
 * are added should match the underlying buffer. Adding elements updates all held elements in the
 * layout
 */
class VertexLayout
{
public:
    explicit VertexLayout(std::initializer_list<VertexAttribute> attributes);
    VertexLayout() = default;

    /// @brief Sets the layout for late initialization
    void SetLayout(std::initializer_list<VertexAttribute> attributes);
    /// @brief Returns the layout
    Vector<VertexElement> GetElements() const;
    /// @brief Returns the stride/size of a single vertex according to this layout
    u32 GetVertexStride() const;
    /// @brief Returns if this layout has the given attribute.
    bool HasAttribute(VertexAttribute attribute) const;

    u32 GetElementOffset(VertexAttribute attribute) const;
    u32 GetElementSize(VertexAttribute attribute) const;

private:
    Vector<VertexElement> m_elements{ };
    HashSet<VertexAttribute> m_attributes{ };
    u32 m_stride{ };
};
} // namespace siren::core

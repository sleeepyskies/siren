#pragma once

#include "core/spch.hpp"
#include "platform/opengl/gl.hpp"

/// @todo refactor this file

namespace siren::core
{
/**
 * @brief Enum listing all allowed Shader Attributes.
 */
enum class VertexAttribute {
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
struct VertexElement {
    /// @brief The name of this attribute
    VertexAttribute attribute{ };
    /// @brief The number of components per vertex attribute
    u32 size{ 0 };
    /// @brief The datatype of this vertex attribute
    GLenum type{ GL_FLOAT }; // only use floats for now, no need to optimise yet
    /// @brief Whether the data is normalized
    bool normalized{ false }; // hardcoded to false for now as I have no use for
    /// @brief The byte offset of the first vertex attribute into the whole VBO
    usize offset{ 0 };
};

/**
 * @brief Describes the full layout of a single Vertex Buffer. Note that the order in which elements
 * are added should match the underlying buffer. Adding elements updates all held elements in the
 * layout
 */
class VertexLayout {
public:
    explicit VertexLayout(std::vector<VertexAttribute>&& attributes);
    VertexLayout() = default;

    /// @brief Sets the layout for late initialization
    auto layout(std::vector<VertexAttribute>&& attributes) -> void;
    /// @brief Returns the layout
    auto elements() const -> std::vector<VertexElement>;
    /// @brief Returns the stride/size of a single vertex according to this layout
    auto vertex_stride() const -> u32;
    /// @brief Returns if this layout has the given attribute.
    auto has_attribute(VertexAttribute attribute) const -> bool;

    auto element_offset(VertexAttribute attribute) const -> u32;
    auto element_size(VertexAttribute attribute) const -> u32;

private:
    std::vector<VertexElement> m_elements{ };
    std::unordered_set<VertexAttribute> m_attributes{ };
    u32 m_stride{ };
};
} // namespace siren::core

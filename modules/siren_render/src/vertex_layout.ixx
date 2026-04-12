module;

#include <libassert/assert.hpp>
#include <vector>
#include <set>

export module siren.render:vertex_layout;

import siren.common;

namespace siren::render {

export enum class DataType {
    Float,
};

/**
 * @brief All possible shader attributes.
 */
export class VertexAttribute {
public:
    enum Enum : u8 {
        Position,
        Normal,
        Tangent,
        Bitangent,
        Texture,
        Color,
    } value;

    /** @brief Returns the number of components this VertexAttribute has. */
    [[nodiscard]] constexpr auto num_components() const -> usize {
        switch (value) {
            case Position:
            case Normal:
            case Tangent:
            case Bitangent: return 3;
            case Texture: return 2;
            case Color: return 4;
        }
        UNREACHABLE("Invalid VertexAttribute encountered.");
    }

    /** @brief Returns the data type of this attribute. */
    [[nodiscard]] constexpr auto type() const -> DataType {
        switch (value) {
            case Position:
            case Normal:
            case Tangent:
            case Bitangent:
            case Texture:
            case Color: return DataType::Float;
        }
        UNREACHABLE("Invalid VertexAttribute encountered.");
    }

    // no explicit by choice
    VertexAttribute(const Enum value) : value(value) { }
    VertexAttribute() = default;

    // no explicit by choice, conversion for switches
    constexpr operator Enum() const { return value; }
};

/** @brief Describes how the GPU should read the data for one attribute from a Vertex Buffer. */
export struct VertexElement {
    /** @brief The name of this attribute */
    VertexAttribute attribute{ };
    /** @brief The number of components per vertex attribute */
    u32 size{ 0 };
    /** @brief Whether the data is normalized */
    bool normalized{ false }; // hardcoded to false for now as I have no use for
    /** @brief The byte offset of the first vertex attribute into the whole VBO */
    usize offset{ 0 };
};

/**
 * @brief Describes the full layout of a single Vertex Buffer. Note that the order in which elements
 * are added should match the underlying buffer. Adding elements updates all held elements in the
 * layout
 */
export class VertexLayout {
public:
    explicit VertexLayout(std::vector<VertexAttribute>&& attributes);
    VertexLayout() = default;

    /** @brief Sets the layout for late initialization */
    auto layout(std::vector<VertexAttribute>&& attributes) -> void;
    /** @brief Returns the layout */
    auto elements() const -> std::vector<VertexElement>;
    /** @brief Returns the stride/size of a single vertex according to this layout */
    auto vertex_stride() const -> u32;
    /** @brief Returns if this layout has the given attribute. */
    auto has_attribute(VertexAttribute attribute) const -> bool;

    auto element_offset(VertexAttribute attribute) const -> u32;
    auto element_size(VertexAttribute attribute) const -> u32;

private:
    std::vector<VertexElement> m_elements{ };
    std::set<VertexAttribute> m_attributes{ };
    u32 m_stride{ };
};

VertexLayout::VertexLayout(std::vector<VertexAttribute>&& attributes) {
    layout(std::forward<std::vector<VertexAttribute>>(attributes));
}

auto VertexLayout::layout(std::vector<VertexAttribute>&& attributes) -> void {
    m_stride = 0;

    for (const auto a : attributes) {
        m_attributes.insert(a);
        const u32 size = a.num_components();

        m_elements.push_back(
            VertexElement{
                .attribute = a,
                .size = size,
                .normalized = false,
                .offset = m_stride,
            }
        );

        m_stride += size * sizeof(float); // for now, everything is a float
    }
}

auto VertexLayout::elements() const -> std::vector<VertexElement> {
    return m_elements;
}

auto VertexLayout::vertex_stride() const -> u32 {
    return m_stride;
}

auto VertexLayout::has_attribute(const VertexAttribute attribute) const -> bool {
    return m_attributes.contains(attribute);
}

auto VertexLayout::element_offset(const VertexAttribute attribute) const -> u32 {
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.offset;
        }
    }
    return 0;
}

auto VertexLayout::element_size(const VertexAttribute attribute) const -> u32 {
    for (const auto& elem : m_elements) {
        if (elem.attribute == attribute) {
            return elem.size;
        }
    }
    return 0;
}

} // namespace siren::render

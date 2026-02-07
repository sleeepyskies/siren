#include "Primitive.hpp"

#include "VertexBufferBuilder.hpp"

#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"
#include "../renderer/resources/Buffer.hpp"

#include "core/assert.hpp"

// many of these generation algorithms have been adapted from three.js
// https://github.com/mrdoob/three.js/tree/dev

namespace siren::core::primitive
{
auto generate(const PrimitiveParams& params, const VertexLayout& layout) -> PrimitiveMeshData {
    auto visitor = [&layout]<typename TArg> (TArg&& args) -> PrimitiveMeshData {
        using T = std::decay_t<TArg>;

        if constexpr (std::is_same_v<T, PlaneParams>) {
            return generate_plane(args, layout);
        } else if constexpr (std::is_same_v<T, CapsuleParams>) {
            return generate_capsule(args, layout);
        } else if constexpr (std::is_same_v<T, CubeParams>) {
            return generate_cube(args, layout);
        }
        SIREN_ASSERT(false, "Invalid PrimitiveParams encountered");
    };

    return std::visit(visitor, params);
}

auto generate_plane(const PlaneParams& params, const VertexLayout& layout) -> PrimitiveMeshData {
    // clamp into local variables
    const float width        = std::clamp(params.width, 0.f, 1000.f);
    const float depth        = std::clamp(params.depth, 0.f, 1000.f);
    const u32 width_segments = std::clamp(params.width_segments, 1u, 128u);
    const u32 depth_segments = std::clamp(params.depth_segments, 1u, 128u);

    std::vector<u32> indices;
    VertexBufferBuilder vbb{ layout };

    const float width_half    = width * 0.5f;
    const float depth_half    = depth * 0.5f;
    const float segment_width = width / width_segments;
    const float segment_depth = depth / depth_segments;

    for (u32 iz = 0; iz <= depth_segments; iz++) {
        const float z = iz * segment_depth - depth_half;
        for (u32 ix = 0; ix <= width_segments; ix++) {
            const float x = ix * segment_width - width_half;
            vbb.push_vertex(
                {
                    .position = { x, 0, z },
                    .normal = { 0, 1, 0 },
                    .texture = {
                        static_cast<float>(ix) / width_segments,
                        static_cast<float>(iz) / depth_segments
                    }
                }
            );
        }
    }

    const u32 row_size = width_segments + 1;

    for (u32 iz = 0; iz < depth_segments; iz++) {
        for (u32 ix = 0; ix < width_segments; ix++) {
            u32 start = iz * row_size + ix;
            indices.push_back(start);
            indices.push_back(start + row_size);
            indices.push_back(start + 1);
            indices.push_back(start + row_size);
            indices.push_back(start + row_size + 1);
            indices.push_back(start + 1);
        }
    }

    return PrimitiveMeshData{
        .vertices = vbb.build(),
        .indices = Buffer{
            std::span(reinterpret_cast<u8*>(indices.data()), indices.size() * sizeof(u32)),
            BufferUsage::Static
        },
        .index_count = (indices.size())
    };
}

auto generate_capsule(const CapsuleParams& params, const VertexLayout& layout) -> PrimitiveMeshData {
    constexpr float PI = glm::pi<float>();

    // enforce constraints
    const float radius         = std::clamp(params.radius, 0.f, 1000.f);
    const float height         = std::clamp(params.height, 0.f, 1000.f);
    const u32 capsule_segments = std::clamp(params.capsuleSegments, 1u, 128u);
    const u32 radial_segments  = std::clamp(params.radialSegments, 3u, 128u);
    const u32 height_segments  = std::clamp(params.heightSegments, 1u, 128u);

    std::vector<u32> indices;
    VertexBufferBuilder vbb{ layout };

    const float half_height          = height / 2;
    const float capsule_arc_length   = PI / 2 * radius;
    const float cylinder_length      = height;
    const float total_arc_length     = 2 * capsule_arc_length + cylinder_length;
    const u32 vertical_segment_count = capsule_segments * 2 + height_segments;
    const u32 vertices_per_row       = radial_segments + 1;

    for (u32 iy = 0; iy <= vertical_segment_count; iy++) {
        float current_arc_length = 0;
        float profile_y          = 0;
        float profile_radius     = 0;
        float normal_y_component = 0;

        if (iy <= capsule_segments) {
            // bottom cap
            const float segment_progress = static_cast<float>(iy) / capsule_segments;
            const float angle            = segment_progress * PI / 2;
            profile_y                    = -half_height - radius * std::cos(angle);
            profile_radius               = radius * std::sin(angle);
            normal_y_component           = -radius * std::cos(angle);
            current_arc_length           = segment_progress * capsule_arc_length;
        } else if (iy <= capsule_segments + height_segments) {
            // middle section
            const float segment_progress = (iy - capsule_segments) / height_segments;
            profile_y                    = -half_height + segment_progress * height;
            profile_radius               = radius;
            normal_y_component           = 0;
            current_arc_length           = capsule_arc_length + segment_progress * cylinder_length;
        } else {
            // top cap
            const float segment_progress = (static_cast<float>(iy) - capsule_segments - height_segments) /
                    capsule_segments;
            const float angle  = segment_progress * PI / 2;
            profile_y          = half_height + radius * std::sin(angle);
            profile_radius     = radius * std::cos(angle);
            normal_y_component = radius * std::sin(angle);
            current_arc_length = capsule_arc_length + cylinder_length + segment_progress * capsule_arc_length;
        }

        const float v = std::max(0.f, std::min(1.f, current_arc_length / total_arc_length));

        // special case for the poles
        float u_offset = 0;

        if (iy == 0) {
            u_offset = 0.5 / radial_segments;
        } else if (iy == vertical_segment_count) {
            u_offset = -0.5 / radial_segments;
        }

        for (u32 ix = 0; ix <= radial_segments; ix++) {
            const float u     = static_cast<float>(ix) / radial_segments;
            const float theta = u * PI * 2;

            const float sin_theta = std::sin(theta);
            const float cos_theta = std::cos(theta);

            vbb.push_vertex(
                {
                    .position = { profile_radius * cos_theta, profile_y, profile_radius * sin_theta },
                    .normal = { -profile_radius * cos_theta, normal_y_component, profile_radius * sin_theta },
                    .texture = { u + u_offset, v }
                }
            );
        }

        if (iy > 0) {
            const u32 prev_index_row = (iy - 1) * vertices_per_row;
            for (u32 ix = 0; ix < radial_segments; ix++) {
                const u32 i1 = prev_index_row + ix;
                const u32 i2 = prev_index_row + ix + 1;
                const u32 i3 = iy * vertices_per_row + ix;
                const u32 i4 = iy * vertices_per_row + ix + 1;

                indices.push_back(i3);
                indices.push_back(i4);
                indices.push_back(i2);
                indices.push_back(i3);
                indices.push_back(i2);
                indices.push_back(i1);
            }
        }
    }

    return PrimitiveMeshData{
        .vertices = vbb.build(),
        .indices = Buffer{
            std::span(reinterpret_cast<u8*>(indices.data()), indices.size() * sizeof(u32)),
            BufferUsage::Static
        },
        .index_count = (indices.size())
    };
}

auto generate_cube(const CubeParams& params, const VertexLayout& layout) -> PrimitiveMeshData {
    VertexBufferBuilder vbb{ layout };
    std::vector<u32> indices;

    const float size      = params.size;
    const u32 height_segs = params.height_segments;
    const u32 width_segs  = params.width_segments;
    const u32 depth_segs  = params.depth_segments;
    const float half_size = size * 0.5f;

    auto add_face = [&] (
        const glm::vec3 origin,
        const glm::vec3 u_dir,
        const glm::vec3 v_dir,
        const u32 u_segs,
        const u32 v_segs
    ) {
        const u32 start_index = vbb.get_size();

        for (u32 y = 0; y <= v_segs; y++) {
            float v = static_cast<float>(y) / v_segs;
            for (u32 x = 0; x <= u_segs; x++) {
                float u             = static_cast<float>(x) / u_segs;
                const glm::vec3 pos = origin + u_dir * (u - 0.5f) * size + v_dir * (v - 0.5f) * size;
                vbb.push_vertex(
                    {
                        .position = pos,
                        .normal = glm::normalize(glm::cross(u_dir, v_dir)),
                        .tangent = glm::normalize(u_dir),
                        .bitangent = glm::normalize(v_dir),
                        .texture = { u, v },
                    }
                );
            }
        }

        for (u32 y = 0; y < v_segs; y++) {
            for (u32 x = 0; x < u_segs; x++) {
                u32 a = start_index + y * (u_segs + 1) + x;
                u32 b = a + u_segs + 1;

                indices.push_back(a);
                indices.push_back(a + 1);
                indices.push_back(b);

                indices.push_back(a + 1);
                indices.push_back(b + 1);
                indices.push_back(b);
            }
        }
    };

    // +X face
    add_face({ half_size, 0, 0 }, { 0, 0, -size }, { 0, size, 0 }, depth_segs, height_segs);
    // -X face
    add_face({ -half_size, 0, 0 }, { 0, 0, size }, { 0, size, 0 }, depth_segs, height_segs);
    // +Y face
    add_face({ 0, half_size, 0 }, { size, 0, 0 }, { 0, 0, -size }, width_segs, depth_segs);
    // -Y face
    add_face({ 0, -half_size, 0 }, { size, 0, 0 }, { 0, 0, size }, width_segs, depth_segs);
    // +Z face
    add_face({ 0, 0, half_size }, { size, 0, 0 }, { 0, size, 0 }, width_segs, height_segs);
    // -Z face
    add_face({ 0, 0, -half_size }, { -size, 0, 0 }, { 0, size, 0 }, width_segs, height_segs);

    return PrimitiveMeshData{
        .vertices = vbb.build(),
        .indices = Buffer{
            std::span(reinterpret_cast<u8*>(indices.data()), indices.size() * sizeof(u32)),
            BufferUsage::Static
        },
        .index_count = (indices.size())
    };
}

auto create_primitive_name(const PrimitiveParams& params) -> std::string {
    // todo: some ID for primitives? Plane_001 etc

    auto visitor = []<typename TArg> (TArg&& arg) -> std::string {
        using T = std::decay_t<TArg>;

        if constexpr (std::is_same_v<T, PlaneParams>) {
            return "Plane";
        } else if constexpr (std::is_same_v<T, CapsuleParams>) {
            return "Capsule";
        } else if constexpr (std::is_same_v<T, CubeParams>) {
            return "Cube";
        }
        SIREN_ASSERT(false, "Invalid PrimitiveParams encountered");
    };

    return std::visit(visitor, params);
}
} // namespace siren::core::primitive

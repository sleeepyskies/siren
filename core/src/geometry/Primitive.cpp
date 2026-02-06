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
std::shared_ptr<PrimitiveMeshData> generate(const PrimitiveParams& params, const VertexLayout& layout) {
    auto visitor = [&layout]<typename TArg> (TArg&& args) -> std::shared_ptr<PrimitiveMeshData> {
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

std::shared_ptr<PrimitiveMeshData> generate_plane(const PlaneParams& params, const VertexLayout& layout) {
    // clamp into local variables
    const float width       = std::clamp(params.width, 0.f, 1000.f);
    const float depth       = std::clamp(params.depth, 0.f, 1000.f);
    const u32 widthSegments = std::clamp(params.widthSegments, 1u, 128u);
    const u32 depthSegments = std::clamp(params.depthSegments, 1u, 128u);

    std::vector<u32> indices;
    VertexBufferBuilder vbb{ layout };

    const float widthHalf    = width * 0.5f;
    const float depthHalf    = depth * 0.5f;
    const float segmentWidth = width / widthSegments;
    const float segmentDepth = depth / depthSegments;

    for (u32 iz = 0; iz <= depthSegments; iz++) {
        const float z = iz * segmentDepth - depthHalf;
        for (u32 ix = 0; ix <= widthSegments; ix++) {
            const float x = ix * segmentWidth - widthHalf;
            vbb.push_vertex(
                {
                    .position = { x, 0, z },
                    .normal = { 0, 1, 0 },
                    .texture = {
                        static_cast<float>(ix) / widthSegments,
                        static_cast<float>(iz) / depthSegments
                    }
                }
            );
        }
    }

    const u32 rowSize = widthSegments + 1;

    for (u32 iz = 0; iz < depthSegments; iz++) {
        for (u32 ix = 0; ix < widthSegments; ix++) {
            u32 start = iz * rowSize + ix;
            indices.push_back(start);
            indices.push_back(start + rowSize);
            indices.push_back(start + 1);
            indices.push_back(start + rowSize);
            indices.push_back(start + rowSize + 1);
            indices.push_back(start + 1);
        }
    }

    auto indexBuffer = std::make_shared<Buffer>(indices.data(), indices.size() * sizeof(u32), BufferUsage::Static);

    return std::make_shared<PrimitiveMeshData>(vbb.build(), indexBuffer, indices.size());
}

std::shared_ptr<PrimitiveMeshData> generate_capsule(const CapsuleParams& params, const VertexLayout& layout) {
    constexpr float PI = glm::pi<float>();

    // enforce constraints
    const float radius        = std::clamp(params.radius, 0.f, 1000.f);
    const float height        = std::clamp(params.height, 0.f, 1000.f);
    const u32 capsuleSegments = std::clamp(params.capsuleSegments, 1u, 128u);
    const u32 radialSegments  = std::clamp(params.radialSegments, 3u, 128u);
    const u32 heightSegments  = std::clamp(params.heightSegments, 1u, 128u);

    std::vector<u32> indices;
    VertexBufferBuilder vbb{ layout };

    const float halfHeight         = height / 2;
    const float capsuleArcLength   = PI / 2 * radius;
    const float cylinderLength     = height;
    const float totalArcLength     = 2 * capsuleArcLength + cylinderLength;
    const u32 verticalSegmentCount = capsuleSegments * 2 + heightSegments;
    const u32 verticesPerRow       = radialSegments + 1;

    for (u32 iy = 0; iy <= verticalSegmentCount; iy++) {
        float currentArcLength = 0;
        float profileY         = 0;
        float profileRadius    = 0;
        float normalYComponent = 0;

        if (iy <= capsuleSegments) {
            // bottom cap
            const float segmentProgress = static_cast<float>(iy) / capsuleSegments;
            const float angle           = segmentProgress * PI / 2;
            profileY                    = -halfHeight - radius * std::cos(angle);
            profileRadius               = radius * std::sin(angle);
            normalYComponent            = -radius * std::cos(angle);
            currentArcLength            = segmentProgress * capsuleArcLength;
        } else if (iy <= capsuleSegments + heightSegments) {
            // middle section
            const float segmentProgress = (iy - capsuleSegments) / heightSegments;
            profileY                    = -halfHeight + segmentProgress * height;
            profileRadius               = radius;
            normalYComponent            = 0;
            currentArcLength            = capsuleArcLength + segmentProgress * cylinderLength;
        } else {
            // top cap
            const float segmentProgress = (static_cast<float>(iy) - capsuleSegments - heightSegments) /
                    capsuleSegments;
            const float angle = segmentProgress * PI / 2;
            profileY          = halfHeight + radius * std::sin(angle);
            profileRadius     = radius * std::cos(angle);
            normalYComponent  = radius * std::sin(angle);
            currentArcLength  = capsuleArcLength + cylinderLength + segmentProgress * capsuleArcLength;
        }

        const float v = std::max(0.f, std::min(1.f, currentArcLength / totalArcLength));

        // special case for the poles
        float uOffset = 0;

        if (iy == 0) {
            uOffset = 0.5 / radialSegments;
        } else if (iy == verticalSegmentCount) {
            uOffset = -0.5 / radialSegments;
        }

        for (u32 ix = 0; ix <= radialSegments; ix++) {
            const float u     = static_cast<float>(ix) / radialSegments;
            const float theta = u * PI * 2;

            const float sinTheta = std::sin(theta);
            const float cosTheta = std::cos(theta);

            vbb.push_vertex(
                {
                    .position = { profileRadius * cosTheta, profileY, profileRadius * sinTheta },
                    .normal = { -profileRadius * cosTheta, normalYComponent, profileRadius * sinTheta },
                    .texture = { u + uOffset, v }
                }
            );
        }

        if (iy > 0) {
            const u32 prevIndexRow = (iy - 1) * verticesPerRow;
            for (u32 ix = 0; ix < radialSegments; ix++) {
                const u32 i1 = prevIndexRow + ix;
                const u32 i2 = prevIndexRow + ix + 1;
                const u32 i3 = iy * verticesPerRow + ix;
                const u32 i4 = iy * verticesPerRow + ix + 1;

                indices.push_back(i3);
                indices.push_back(i4);
                indices.push_back(i2);
                indices.push_back(i3);
                indices.push_back(i2);
                indices.push_back(i1);
            }
        }
    }

    const auto indexBuffer = std::make_shared<Buffer>(
        indices.data(),
        indices.size() * sizeof(u32),
        BufferUsage::Static
    );
    return std::make_shared<PrimitiveMeshData>(vbb.build(), indexBuffer, indices.size());
}

std::shared_ptr<PrimitiveMeshData> generate_cube(const CubeParams& params, const VertexLayout& layout) {
    VertexBufferBuilder vbb{ layout };
    std::vector<u32> indices;

    const float size     = params.size;
    const u32 heightSegs = params.heightSegments;
    const u32 widthSegs  = params.widthSegments;
    const u32 depthSegs  = params.depthSegments;
    const float halfSize = size * 0.5f;

    auto addFace = [&] (
        const glm::vec3 origin,
        const glm::vec3 uDir,
        const glm::vec3 vDir,
        const u32 uSegs,
        const u32 vSegs
    ) {
        const u32 startIndex = vbb.get_size();

        for (u32 y = 0; y <= vSegs; y++) {
            float v = static_cast<float>(y) / vSegs;
            for (u32 x = 0; x <= uSegs; x++) {
                float u             = static_cast<float>(x) / uSegs;
                const glm::vec3 pos = origin + uDir * (u - 0.5f) * size + vDir * (v - 0.5f) * size;
                vbb.push_vertex(
                    {
                        .position = pos,
                        .normal = glm::normalize(glm::cross(uDir, vDir)),
                        .tangent = glm::normalize(uDir),
                        .bitangent = glm::normalize(vDir),
                        .texture = { u, v },
                    }
                );
            }
        }

        for (u32 y = 0; y < vSegs; y++) {
            for (u32 x = 0; x < uSegs; x++) {
                u32 a = startIndex + y * (uSegs + 1) + x;
                u32 b = a + uSegs + 1;

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
    addFace({ halfSize, 0, 0 }, { 0, 0, -size }, { 0, size, 0 }, depthSegs, heightSegs);
    // -X face
    addFace({ -halfSize, 0, 0 }, { 0, 0, size }, { 0, size, 0 }, depthSegs, heightSegs);
    // +Y face
    addFace({ 0, halfSize, 0 }, { size, 0, 0 }, { 0, 0, -size }, widthSegs, depthSegs);
    // -Y face
    addFace({ 0, -halfSize, 0 }, { size, 0, 0 }, { 0, 0, size }, widthSegs, depthSegs);
    // +Z face
    addFace({ 0, 0, halfSize }, { size, 0, 0 }, { 0, size, 0 }, widthSegs, heightSegs);
    // -Z face
    addFace({ 0, 0, -halfSize }, { -size, 0, 0 }, { 0, size, 0 }, widthSegs, heightSegs);

    auto indexBuffer = std::make_shared<Buffer>(indices.data(), indices.size() * sizeof(u32), BufferUsage::Static);
    return std::make_shared<PrimitiveMeshData>(vbb.build(), indexBuffer, indices.size());
}

std::string CreatePrimitiveName(const PrimitiveParams& params) {
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

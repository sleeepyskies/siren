#include "PrimitiveMesh.hpp"

#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"
#include "renderer/buffer/VertexArray.hpp"

namespace siren::core::primitive
{

Ref<VertexArray> generate(const PrimitiveParams& params)
{
    auto visitor = []<typename TArg>(TArg&& arg) -> Ref<VertexArray> {
        using T = std::decay_t<TArg>;

        if constexpr (std::is_same_v<T, PlaneParams>) {
            return generatePlane(arg);
        } else if constexpr (std::is_same_v<T, CapsuleParams>) {
            return generateCapsule(arg);
        }
    };

    return std::visit(visitor, params);
}

Ref<VertexArray> generatePlane(const PlaneParams& params)
{
    std::vector<glm::vec3> positions{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> uvs{};

    std::vector<u32> indices;

    const u32 wSeg    = params.widthSegments;
    const u32 dSeg    = params.depthSegments;
    const float wHalf = params.width * 0.5f;
    const float dHalf = params.depth * 0.5f;

    for (u32 z = 0; z <= dSeg; z++) {
        float dz = (static_cast<float>(z) / dSeg) * params.depth - dHalf;
        for (u32 x = 0; x <= wSeg; x++) {
            float dx = (static_cast<float>(x) / wSeg) * params.width - wHalf;
            positions.push_back({dx, 0.0f, dz});
            normals.push_back({0.0f, 1.0f, 0.0f});
            uvs.push_back({static_cast<float>(x) / wSeg, static_cast<float>(z) / dSeg});
        }
    }

    for (u32 z = 0; z < dSeg; z++) {
        for (u32 x = 0; x < wSeg; x++) {
            u32 start = z * (wSeg + 1) + x;
            indices.push_back(start);
            indices.push_back(start + wSeg + 1);
            indices.push_back(start + 1);

            indices.push_back(start + 1);
            indices.push_back(start + wSeg + 1);
            indices.push_back(start + wSeg + 2);
        }
    }

    VertexData vertexData;
    vertexData.positions  = std::move(positions);
    vertexData.normals    = std::move(normals);
    vertexData.textureUvs = std::move(uvs);

    const auto vertexBuffer = createRef<VertexBuffer>(vertexData, BufferUsage::STATIC);
    const auto indexBuffer  = createRef<IndexBuffer>(indices);
    const auto vertexArray  = createRef<VertexArray>();
    vertexArray->linkVertexBuffer(vertexBuffer);
    vertexArray->linkIndexBuffer(indexBuffer);

    return vertexArray;
}

Ref<VertexArray> generateCapsule(const CapsuleParams& params)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    std::vector<u32> indices;

    const u32 segments     = params.segments;
    const float radius     = params.radius;
    const float halfHeight = params.height * 0.5f;

    // cylinder body
    for (u32 y = 0; y <= segments; y++) {
        float v    = static_cast<float>(y) / segments;
        float yPos = v * params.height - halfHeight;
        for (u32 i = 0; i <= segments; i++) {
            float u     = static_cast<float>(i) / segments;
            float theta = u * 2.0f * glm::pi<float>();
            float x     = glm::cos(theta) * radius;
            float z     = glm::sin(theta) * radius;
            positions.push_back({x, yPos, z});
            normals.push_back(glm::normalize(glm::vec3(x, 0.0f, z)));
            uvs.push_back({u, v});
        }
    }

    for (u32 y = 0; y < segments; y++) {
        for (u32 i = 0; i < segments; i++) {
            u32 start = y * (segments + 1) + i;
            indices.push_back(start);
            indices.push_back(start + segments + 1);
            indices.push_back(start + 1);

            indices.push_back(start + 1);
            indices.push_back(start + segments + 1);
            indices.push_back(start + segments + 2);
        }
    }

    auto addHemisphere = [&](const float yOffset, const bool invert) {
        const u32 baseIndex = static_cast<u32>(positions.size());
        for (u32 lat = 1; lat <= segments / 2; lat++) {
            float phi = glm::pi<float>() * 0.5f * (static_cast<float>(lat) / (segments / 2));
            float r   = radius * glm::cos(phi);
            float y   = radius * glm::sin(phi);
            y         = invert ? -y : y;
            y += yOffset;

            for (u32 i = 0; i <= segments; i++) {
                float u     = static_cast<float>(i) / segments;
                float theta = u * 2.0f * glm::pi<float>();
                float x     = glm::cos(theta) * r;
                float z     = glm::sin(theta) * r;
                positions.push_back({x, y, z});
                glm::vec3 n = glm::normalize(glm::vec3(x, invert ? -y + yOffset : y - yOffset, z));
                normals.push_back(n);
                uvs.push_back({u,
                               invert ? 0.0f + static_cast<float>(lat) / (segments / 2)
                                      : 1.0f - static_cast<float>(lat) / (segments / 2)});
            }
        }

        // hemisphere indices
        for (u32 lat = 0; lat < segments / 2; lat++) {
            for (u32 i = 0; i < segments; i++) {
                u32 start = baseIndex + lat * (segments + 1) + i;
                u32 next  = start + segments + 1;

                if (invert) {
                    indices.push_back(start + 1);
                    indices.push_back(next);
                    indices.push_back(start);

                    indices.push_back(start + segments + 2);
                    indices.push_back(next);
                    indices.push_back(start + 1);
                } else {
                    indices.push_back(start);
                    indices.push_back(next);
                    indices.push_back(start + 1);

                    indices.push_back(start + 1);
                    indices.push_back(next);
                    indices.push_back(next + 1);
                }
            }
        }
    };

    addHemisphere(halfHeight, false);
    addHemisphere(-halfHeight, true);

    VertexData vertexData;
    vertexData.positions  = std::move(positions);
    vertexData.normals    = std::move(normals);
    vertexData.textureUvs = std::move(uvs);

    const auto vertexBuffer = createRef<VertexBuffer>(vertexData, BufferUsage::STATIC);
    const auto indexBuffer  = createRef<IndexBuffer>(indices);
    const auto vertexArray  = createRef<VertexArray>();
    vertexArray->linkVertexBuffer(vertexBuffer);
    vertexArray->linkIndexBuffer(indexBuffer);

    return vertexArray;
}

} // namespace siren::core::primitive

#include "primitive.hpp"

#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"
#include "renderer/buffer/VertexArray.hpp"


namespace siren::core::primitive
{

Ref<VertexArray> generate(const PrimitiveParams& params)
{
    auto visitor = []<typename TArg> (TArg&& arg) -> Ref<VertexArray> {
        using T = std::decay_t<TArg>;

        if constexpr (std::is_same_v<T, PlaneParams>) {
            return generatePlane(arg);
        } else if constexpr (std::is_same_v<T, CapsuleParams>) {
            return generateCapsule(arg);
        }
        SirenAssert(false, "Invalid PrimitiveParams encountered");
    };

    return std::visit(visitor, params);
}

Ref<VertexArray> generatePlane(const PlaneParams& params)
{
    std::vector<glm::vec3> positions{ };
    std::vector<glm::vec3> normals{ };
    std::vector<glm::vec2> uvs{ };

    std::vector<u32> indices;

    const u32 wSeg    = params.widthSegments;
    const u32 dSeg    = params.depthSegments;
    const float wHalf = params.width * 0.5f;
    const float dHalf = params.depth * 0.5f;

    for (u32 z = 0; z <= dSeg; z++) {
        float dz = (static_cast<float>(z) / dSeg) * params.depth - dHalf;
        for (u32 x = 0; x <= wSeg; x++) {
            float dx = (static_cast<float>(x) / wSeg) * params.width - wHalf;
            positions.push_back({ dx, 0.0f, dz });
            normals.push_back({ 0.0f, 1.0f, 0.0f });
            uvs.push_back({ static_cast<float>(x) / wSeg, static_cast<float>(z) / dSeg });
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
            positions.push_back({ x, yPos, z });
            normals.push_back(glm::normalize(glm::vec3(x, 0.0f, z)));
            uvs.push_back({ u, v });
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

    auto addHemisphere = [&] (float centerY, bool invert) {
        glm::vec3 center(0, centerY, 0);
        float sign = invert ? -1.0f : 1.0f;

        for (u32 lat = 0; lat <= segments / 2; lat++) {
            float v   = (float)lat / (segments / 2);
            float phi = v * glm::half_pi<float>();

            float y = sin(phi) * radius * sign;
            float r = cos(phi) * radius;

            for (u32 i = 0; i <= segments; i++) {
                float u     = (float)i / segments;
                float theta = u * glm::two_pi<float>();

                float x = cos(theta) * r;
                float z = sin(theta) * r;

                glm::vec3 p = { x, centerY + y, z };
                positions.push_back(p);
                normals.push_back(normalize(p - center));
                uvs.push_back({ u, invert ? v : 1.0f - v });
            }
        }

        u32 base = (u32)positions.size() - (segments / 2 + 1) * (segments + 1);
        for (u32 lat = 0; lat < segments / 2; lat++) {
            for (u32 i = 0; i < segments; i++) {
                u32 a = base + lat * (segments + 1) + i;
                u32 b = a + segments + 1;

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(a + 1);

                indices.push_back(a + 1);
                indices.push_back(b);
                indices.push_back(b + 1);
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

std::string createPrimitiveName(const PrimitiveParams& params)
{
    // todo: some ID for primitives? Plane_001 etc

    auto visitor = []<typename TArg> (TArg&& arg) -> std::string {
        using T = std::decay_t<TArg>;

        if constexpr (std::is_same_v<T, PlaneParams>) {
            return "Plane";
        } else if constexpr (std::is_same_v<T, CapsuleParams>) {
            return "Capsule3D";
        }
        SirenAssert(false, "Invalid PrimitiveParams encountered");
    };

    return std::visit(visitor, params);
}

} // namespace siren::core::primitive

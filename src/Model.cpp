#include "Model.hpp"

Model::Model(const sobj::OBJData& data, const BufferUsage usage) : m_name(data.name)
{
    /*std::vector<Vertex> vertices{};
    std::vector<std::vector<uint32_t>> submeshIndices{};
    std::vector<std::string> names{};
    std::vector<ref<Texture2D>> textures{};

    vertices.reserve(data.positions.size() / 3);

    size_t instance = 0;
    for (const auto& mesh : data.meshes) {
        if (mesh.faces.empty()) continue;

        std::vector<uint32_t> subIndices{};
        names.emplace_back(mesh.name);
        if (mesh.material.diffuseMap.bytes.size() > 0) {
            const auto [name, bytes, width, height, channels] = mesh.material.diffuseMap;
            textures.emplace_back(makeRef<Texture2D>(bytes, width, height, channels));
        } else {
            textures.emplace_back(nullptr);
        }
        for (const auto& face : mesh.faces) {
            const bool hasColor  = !face.colorIndices.empty();
            const bool hasNormal = !face.normalIndices.empty();
            const bool hasUV     = !face.uvIndices.empty();

            for (size_t i = 0; i < face.numVertices(); i++) {

                glm::vec3 position{
                    data.positions[face.positionIndices[i] * 3 + 0],
                    data.positions[face.positionIndices[i] * 3 + 1],
                    data.positions[face.positionIndices[i] * 3 + 2],
                };
                glm::vec3 color  = !hasColor ? glm::vec3{ 0 }
                                             : glm::vec3{
                                                  data.colors[face.colorIndices[i] * 3 + 0],
                                                  data.colors[face.colorIndices[i] * 3 + 1],
                                                  data.colors[face.colorIndices[i] * 3 + 2],
                                              };
                glm::vec3 normal = !hasNormal ? glm::vec3{ 0 }
                                              : glm::vec3{
                                                    data.normals[face.normalIndices[i] * 3 + 0],
                                                    data.normals[face.normalIndices[i] * 3 + 1],
                                                    data.normals[face.normalIndices[i] * 3 + 2],
                                                };
                glm::vec2 uv     = !hasUV ? glm::vec2{ 0 }
                                          : glm::vec2{
                                            data.textureUVs[face.uvIndices[i] * 2 + 0],
                                            data.textureUVs[face.uvIndices[i] * 2 + 1],
                                        };

                vertices.emplace_back(position, color, normal, uv);
                subIndices.push_back(instance++);
            }
        }
        submeshIndices.push_back(subIndices);
    }

    m_vertices = vertices;

    // calling shrink_to_fit after upload can cause memory issues
    m_meshes.shrink_to_fit();
    m_vertices.shrink_to_fit();
    m_vbo.uploadData(m_vertices, usage);

    // TODO rework this logic, really ugly. Maybe rewrite the Mesh Constructor? have an init?
    for (int i = 0; i < submeshIndices.size(); ++i) {
        m_meshes.push_back(makeUref<Mesh>(submeshIndices[i], m_vbo, usage, names[i]));
        // set textures of meshes
        if (textures[i]) m_meshes.back()->setDiffuseMap(textures[i]);
    }*/
}

void Model::bind() const
{
    for (const auto& mesh : m_meshes) {
        mesh->bind();
    }
}

void Model::draw() const
{
    for (const auto& mesh : m_meshes) {
        mesh->bind();
        mesh->draw();
    }
}

void Model::translate(const glm::vec3 translation)
{
    m_globalTransform = glm::translate(m_globalTransform, translation);
}

void Model::scale(const glm::vec3 scale)
{
    m_globalTransform = glm::scale(m_globalTransform, scale);
}

void Model::scale(const float scalar)
{
    scale(glm::vec3(scalar));
}

void Model::rotateDeg(const glm::vec3 axis, const float degrees)
{
    m_globalTransform = glm::rotate(m_globalTransform, glm::radians(degrees), axis);
}

void Model::rotateRad(const glm::vec3 axis, const float radians)
{
    m_globalTransform = glm::rotate(m_globalTransform, radians, axis);
}

glm::mat4 Model::getTransform() const
{
    return m_globalTransform;
}

#include "Mesh.hpp"

Mesh::Mesh(const std::vector<uint32_t>& indices, const VBO& vbo, const BufferUsage usage,
           const std::string& name)
    : m_name(name)
{
    m_indices = indices;
    m_count   = indices.size();
    m_indices.shrink_to_fit();

    m_vao.bind();

    uint32_t index = 0;

    // position vec3
    m_vao.linkAttribute(vbo, { index++, 3, GL_FLOAT, false, sizeof(Vertex), 0 });
    // color vec3
    m_vao.linkAttribute(vbo, { index++, 3, GL_FLOAT, false, sizeof(Vertex), sizeof(glm::vec3) });
    // normal vec3
    m_vao.linkAttribute(vbo,
                        { index++, 3, GL_FLOAT, false, sizeof(Vertex), sizeof(glm::vec3) * 2 });
    // textureUV vec2
    m_vao.linkAttribute(vbo,
                        { index++, 2, GL_FLOAT, false, sizeof(Vertex), sizeof(glm::vec3) * 3 });

    m_ebo.uploadIndices(m_indices, usage);
    m_vao.unbind();
}

void Mesh::setDiffuseMap(const ref<Texture2D>& diffuseMap)
{
    m_diffuseMap = diffuseMap;
}

void Mesh::draw() const
{
    bind();
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    m_vao.unbind();
}

void Mesh::bind() const
{
    if (m_diffuseMap) m_diffuseMap->bind(0);
}

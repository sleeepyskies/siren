#include "Mesh.hpp"

#include "renderer/Shaders.hpp"

namespace core
{

Mesh::Mesh(const std::vector<uint32_t>& indices, const std::string& name)
    : m_indices(indices), m_count(indices.size()), m_name(name)
{
    m_indices.shrink_to_fit();
}

void Mesh::init(const VBO& vbo, const BufferUsage usage)
{
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

glm::mat4 Mesh::modelMatrix() const
{
    return m_transform;
}

void Mesh::setMaterial(const ref<Material>& material)
{
    m_material = material;
}

void Mesh::draw() const
{
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    m_vao.unbind();
}

void Mesh::bind(const Shaders& shaders) const
{
    // basically setup all uniforms, may end up binding
    // materials lots of times in a row, but would require
    // refactoring scene in general, maybe index based instead
    // of pointer based?
    if (m_material) m_material->bind(shaders);
}

} // namespace core

#pragma once

#include "renderer/Buffer.hpp"
#include "scene/Material.hpp"
#include "utilities/spch.hpp"

namespace core
{

class Mesh
{
public:
    Mesh(const std::vector<uint32_t>& indices, const std::string& name);

    void init(const renderer::VBO& vbo, renderer::BufferUsage usage);
    void setMaterial(const ref<Material>& material);

    glm::mat4 modelMatrix() const;

    void draw() const;
    void bind(const renderer::Shaders& shaders) const;

private:
    std::vector<uint32_t> m_indices{};
    size_t m_count = 0;
    std::string m_name{};

    glm::mat4 m_transform{ 1.f };

    ref<Material> m_material = nullptr;

    renderer::EBO m_ebo{};
    renderer::VAO m_vao{};
};

} // namespace core

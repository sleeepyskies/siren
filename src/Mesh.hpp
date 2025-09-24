#pragma once

#include "Material.hpp"
#include "Shaders.hpp"
#include "types.hpp"
#include <Buffer.hpp>
#include <Texture.hpp>
#include <glm/matrix.hpp>
#include <slog.hpp>

class Mesh
{
public:
    Mesh(const std::vector<uint32_t>& indices, const std::string& name);

    void init(const VBO& vbo, BufferUsage usage);
    void setMaterial(const ref<Material>& material);

    glm::mat4 modelMatrix() const;

    void draw() const;
    void bind(const Shaders& shaders) const;

private:
    std::vector<uint32_t> m_indices{};
    size_t m_count = 0;
    std::string m_name{};

    glm::mat4 m_transform{ 1.f };

    ref<Material> m_material = nullptr;

    EBO m_ebo{};
    VAO m_vao{};
};

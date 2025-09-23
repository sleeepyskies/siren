#pragma once

#include "types.hpp"

#include <Buffer.hpp>
#include <Texture.hpp>
#include <glm/matrix.hpp>
#include <slog.hpp>

class Mesh
{
public:
    Mesh(const std::vector<uint32_t>& indices, const VBO& vbo, BufferUsage usage,
         const std::string& name);

    void draw() const;
    void bind() const;

    void setDiffuseMap(const ref<Texture2D>& diffuseMap);

private:
    std::vector<uint32_t> m_indices{};
    size_t m_count = 0;
    std::string m_name{};

    glm::mat4 m_transform{ 1.f };
    ref<Texture2D> m_diffuseMap = nullptr;

    EBO m_ebo{};
    VAO m_vao{};
};

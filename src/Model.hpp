#pragma once

#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "serialization/sobj.hpp"
#include "types.hpp"
#include <glm/matrix.hpp>
#include <slog.hpp>

class Model
{
public:
    Model(const sobj::OBJData& data, BufferUsage usage);
    ~Model() = default;

    void draw() const;
    void bind() const;

    void translate(glm::vec3 translation);
    void scale(glm::vec3 scale);
    void scale(float scalar);
    void rotateDeg(glm::vec3 axis, float degrees);
    void rotateRad(glm::vec3 axis, float radians);
    glm::mat4 getTransform() const;

private:
    std::vector<Vertex> m_vertices{};
    std::vector<uref<Mesh>> m_meshes{};
    std::string m_name{};

    glm::mat4 m_globalTransform{ 1.f };

    VBO m_vbo{};
};

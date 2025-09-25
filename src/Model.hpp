#pragma once

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Shaders.hpp"
#include "Vertex.hpp"
#include "Window.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "sobj.hpp"
#include "types.hpp"
#include <glm/matrix.hpp>
#include <slog.hpp>

class Model
{
public:
    // TODO: not great passing width and height in here...
    Model(const sobj::OBJData& data, BufferUsage usage, int width, int height);
    ~Model() = default;

    void draw(const Shaders& shaders) const;

    void translate(glm::vec3 translation);
    void scale(glm::vec3 scale);
    void scale(float scalar);
    void rotateDeg(glm::vec3 axis, float degrees);
    void rotateRad(glm::vec3 axis, float radians);
    glm::mat4 getTransform() const;

private:
    std::string m_name{};
    std::vector<Vertex> m_vertices{};
    std::vector<uref<Mesh>> m_meshes{};

    glm::mat4 m_globalTransform{ 1.f };

    VBO m_vbo{};
};

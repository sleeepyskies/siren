#pragma once

#include "renderer/Buffer.hpp"
#include "renderer/Shaders.hpp"
#include "renderer/Vertex.hpp"
#include "scene/Mesh.hpp"
#include "sobj.hpp"
#include "utilities/spch.hpp"

namespace core
{

class Model
{
public:
    Model(const sobj::OBJData& data, BufferUsage usage);
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

} // namespace core

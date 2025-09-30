#pragma once

#include "geometry/Mesh.hpp"
#include "renderer/Renderer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class Model
{
public:
    Model();
    ~Model();

    void addMesh(const Mesh& mesh);

    renderer::VertexArray getVertexArray() const
    {
        return m_vertexArray;
    };
    glm::mat4 getGlobalTransform() const
    {
        return m_globalTransform;
    }

private:
    glm::mat4 m_globalTransform{ 1 };
    std::vector<Mesh> m_meshes{};

    renderer::VertexArray m_vertexArray{};
};

} // namespace core::geometry

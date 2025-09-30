#pragma once

#include "geometry/Material.hpp"
#include "renderer/Buffer.hpp"
#include "renderer/Renderer.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

class Mesh
{
public:
    Mesh();
    ~Mesh();

private:
    std::string name;
    Material m_material;
    renderer::VertexBuffer m_vertexBuffer{};
    renderer::IndexBuffer m_indexBuffer{};
};

} // namespace core::geometry

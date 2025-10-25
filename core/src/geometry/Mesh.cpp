#include "Mesh.hpp"

namespace siren::core
{

void Mesh::addSurface(const Surface& surface)
{
    m_surfaces.push_back(surface);
}

const std::vector<Mesh::Surface>& Mesh::getSurfaces() const
{
    return m_surfaces;
}

} // namespace siren::core

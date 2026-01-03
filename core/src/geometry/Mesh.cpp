#include "Mesh.hpp"


namespace siren::core
{
void Mesh::AddSurface(const Surface& surface)
{
    m_surfaces.push_back(surface);
}

const Vector<Mesh::Surface>& Mesh::GetSurfaces() const
{
    return m_surfaces;
}
} // namespace siren::core

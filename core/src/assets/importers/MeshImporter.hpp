#pragma once

#include "geometry/Mesh.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{
class MeshImporter
{
public:
    static Ref<Mesh> importModel(const Path& path);
};

} // namespace siren::core

#pragma once

#include "geometry/Model.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
{

class OBJImporter
{
public:
    static Ref<geometry::Model> importOBJ(const fs::path& path);
};

} // namespace siren::assets

#pragma once

#include "geometry/Model.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
{
class ModelImporter
{
public:
    static Ref<geometry::Model> importModel(const fs::path& path);
};

} // namespace siren::assets

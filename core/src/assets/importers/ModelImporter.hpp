#pragma once

#include "geometry/Model.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
{
class ModelImporter
{
public:
    static Ref<geometry::Model> importModel(const Path& path);
};

} // namespace siren::assets

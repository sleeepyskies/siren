#pragma once

#include "geometry/Model.hpp"
#include "utilities/spch.hpp"

namespace siren::assets::ModelImporter
{

Ref<geometry::Model> importModel(const fs::path& path);

Ref<geometry::Model> importModelFromGLTF(const fs::path& path);

Ref<geometry::Model> importModelFromOBJ(const fs::path& path);

} // namespace siren::assets::ModelImporter

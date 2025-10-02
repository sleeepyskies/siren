#pragma once

#include "geometry/Model.hpp"
#include "utilities/spch.hpp"

namespace core::assets::ModelImporter
{

inline Ref<geometry::Model> importModel(const fs::path& path);

inline Ref<geometry::Model> importModelFromGLTF(const fs::path& path);

inline Ref<geometry::Model> importModelFromOBJ(const fs::path& path);

} // namespace core::assets::ModelImporter

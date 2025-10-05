#pragma once

#include "geometry/Model.hpp"
#include "utilities/spch.hpp"

namespace siren::assets
{
class ModelImporter
{
public:
    static Ref<geometry::Model> importModel(const fs::path& path);

private:
    static Ref<geometry::Model> importModelFromGLTF(const fs::path& path);
    static Ref<geometry::Model> importModelFromOBJ(const fs::path& path);
};

} // namespace siren::assets

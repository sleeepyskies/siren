#pragma once
#include <geometry/Model.hpp>

namespace siren::assets
{

class GLTFImporter
{
public:
    static Ref<geometry::Model> importGLTF(const fs::path& path);
};

} // namespace siren::assets

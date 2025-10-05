#include "ModelImporter.hpp"
#include "model/GLTFImporter.hpp"
#include "model/OBJImporter.hpp"
#include "tiny_gltf.h"

namespace siren::assets
{
using ImportModelFn = std::function<Ref<geometry::Model>(const fs::path&)>;
static std::unordered_map<std::string, ImportModelFn> extensionToImportFn = {
    // gltf files
    { ".gltf", GLTFImporter::importGLTF },
    { ".glb", GLTFImporter::importGLTF },
    // obj
    { ".obj", OBJImporter::importOBJ },
};

Ref<geometry::Model> ModelImporter::importModel(const fs::path& path)
{
    if (!exists(path)) {
        err("File does not exist at {}", path.string());
        return nullptr;
    }

    const std::string extension = path.extension().string();
    if (!extensionToImportFn.contains(extension)) {
        err("File with invalid extension provided: {}", path.string());
        return nullptr;
    }

    return extensionToImportFn[extension](path);
}
} // namespace siren::assets

#pragma once

#include "geometry/Mesh.hpp"
#include "utilities/spch.hpp"

namespace core::assets
{

using ModelID = size_t;

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    void init(const fs::path& workingDirectory);
    void shutdown();

    /// @brief Loads the model from the path relative to the asset directory
    ModelID loadModelFromRelativePath(const fs::path& relativePath);
    ref<geometry::Mesh> getModelByID(ModelID id) const;

private:
    fs::path m_workingDirectory = "";
    fs::path m_assetDirectory   = "";
    fs::path m_modelDirectory   = "";

    ModelID hashModelPath(const std::string& model) const;

    std::unordered_map<ModelID, ref<geometry::Mesh>> m_loadedModels{};

    /// @brief Loads the model from the absolute path. performs no verification
    ref<geometry::Mesh> loadModelFromAbsolutePath(const fs::path& absolutePath);
};

} // namespace core::assets

/**
 * @file TextureImporter.hpp
 */
#pragma once
#include "renderer/Texture.hpp"

class aiScene;
class aiString;


namespace siren::core
{

/**
 * @brief Used to import textures.
 * A new TextureImporter instance should be instantiated for each import.
 */
class TextureImporter
{
public:
    /// @brief Creates a new TextureImporter to load from a filepath.
    static TextureImporter create(const Path& path);
    /// @brief Creates a new TextureImporter to from Assimp. Used mainly by @ref MeshImporter.
    static TextureImporter create(const aiScene* aiScene, const aiString& aiString);
    TextureImporter& setSampler(const TextureSampler& sampler);

    /// @brief Loads and returns the Texture2D. Returns nullptr on fail.
    Ref<Texture2D> load2D();
    Ref<TextureCubeMap> loadCubeMap();

private:
    /**
     * @brief Struct holding relevant source data needed for parsing a texture from Assimp.
     */
    struct AssimpSource
    {
        const aiScene* aiScene;
        const aiString& aiString;
    };

    explicit TextureImporter(const Path& path);
    explicit TextureImporter(const AssimpSource& source);

    std::variant<Path, AssimpSource> m_source;
    TextureSampler m_sampler{ };

    Ref<Texture2D> loadFromPath() const;
    Ref<Texture2D> loadFromAssimp();
};

} // namespace siren::assets::importer

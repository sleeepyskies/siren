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
    static TextureImporter Create(const Path& path);
    /// @brief Creates a new TextureImporter to from Assimp. Used mainly by @ref MeshImporter.
    static TextureImporter Create(const aiScene* aiScene, const aiString& aiString);
    /// @brief Sets a custom sampler for the texture.
    TextureImporter& SetSampler(const TextureSampler& sampler);
    /// @brief Sets if to interpret the image data as srgb or not. Defaults to false.
    TextureImporter& SetTextureFormat(ImageFormat format);

    /// @brief Loads and returns the Texture2D. Returns nullptr on fail.
    Ref<Texture2D> Load2D();
    /// @brief Loads and returns the TextureCubeMap. Returns nullptr on fail.
    Ref<TextureCubeMap> LoadCubeMap();

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
    ImageFormat m_format = ImageFormat::Color8;

    Ref<Texture2D> LoadFromPath() const;
    Ref<Texture2D> LoadFromAssimp();
};
} // namespace siren::assets::importer

#pragma once

#include "MaterialKey.hpp"
#include "assets/Asset.hpp"
#include "renderer/Texture.hpp"
#include "utilities/spch.hpp"

namespace siren::renderer
{

/// @brief An array holding all textures, using the TextureType enum as a key
using TextureArray = std::array<Ref<Texture2D>, static_cast<std::size_t>(TextureType::MAX_TEXTURE)>;

/**
 * @brief A material represents uniform data that is used in a shader
 */
struct Material final : assets::Asset {
    ASSET_TYPE(assets::AssetType::MATERIAL);

    explicit Material(const std::string& name) : Asset(name)
    {
    }
    ~Material() override = default;

    /// @brief Returns this material's MaterialKey for identifying material configurations
    MaterialKey generateMaterialKey() const;

    /// @brief Checks if this Material has a texture of the given type.
    bool hasTexture(TextureType type) const;
    /// @brief Returns the texture of the given type, or nullptr if there is none.
    Ref<Texture2D> getTexture(TextureType type) const;

    // Material params

    glm::vec4 baseColor{ 1 };
    float metallic{ 0 };  // assume materials are by default non metallic
    float roughness{ 1 }; // assume surfaces are by default fully diffuse aka very rough
    glm::vec3 emission{ 0 };
    float occlusion{ 1 };
    float normalScale{ 1 };

    TextureArray textureArray{};
};

} // namespace siren::renderer

/**
 * @file Material.hpp
 */
#pragma once

#include "MaterialKey.hpp"
#include "assets/Asset.hpp"
#include "renderer/Texture.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

/**
 * @brief A PBR material. Follows the metallic-roughness single texture standard.
 */
struct Material final : Asset {
    ASSET_TYPE(AssetType::MATERIAL);

    /**
     * @brief All possible textures the Material can have.
     */
    enum class TextureType : size_t {
        BASE_COLOR,
        METALLIC_ROUGHNESS,
        NORMAL,
        EMISSION,
        OCCLUSION,

        MAX_TEXTURE, // amount of texture types, don't actually use
    };

    /**
     * @brief Alpha blending mode for materials.
     */
    enum class AlphaMode { OPAQUE, BLEND, MASK };

    explicit Material(const std::string& name) : Asset(name) {}
    ~Material() override = default;

    /// @brief Returns this material's @ref MaterialKey.
    MaterialKey getMaterialKey() const;
    /// @brief Invalidates this material's @ref MaterialKey. Used when updating uniforms.
    void invalidateMaterialKey() const;

    /// @brief Checks whether this material has the given @ref TextureType.
    bool hasTexture(TextureType type) const;
    /// @brief Sets the given @ref TextureType.
    void setTexture(TextureType type, AssetHandle textureHandle);
    /// @brief Returns the @ref AssetHandle of the given @ref TextureType.
    Maybe<AssetHandle> getTexture(TextureType type, AssetHandle textureHandle) const;

    // Shader

    /// @brief The @ref AssetHandle of this material's shader.
    AssetHandle shaderHandle = AssetHandle::invalid();

    // PBR parameters

    /// @brief Base color/albedo of the material. RGBA format.
    glm::vec4 baseColor    = glm::vec4(1.0f);
    /// @brief Metallic factor [0,1]. 0 = dielectric, 1 = metal.
    float metallic         = 0;
    /// @brief Roughness factor [0, 1]. 0 = glossy, 1 = rough.
    float roughness        = 1;
    /// @brief Emissive color RGB.
    glm::vec3 emissive     = glm::vec3(0);
    /// @brief Ambient occlusion factor [0, 1]. 0 = full occlusion, 1 = no occlusion.
    float ambientOcclusion = 1;
    /// @brief Normal map scale. 0 = no effect, 1 = full effect.
    float normalScale      = 1;
    /// @brief Alpha blending mode.
    AlphaMode alphaMode    = AlphaMode::OPAQUE;
    /// @brief Cutoff threshold for AlphaMode::MASK. alpha < alphaCutoff is discarded.
    float alphaCutoff      = 0.5;
    /// @brief Enable/Disable double sided rendering.
    bool doubleSided       = false;

private:
    /// @brief Array holding all texture handles.
    std::array<AssetHandle, static_cast<size_t>(TextureType::MAX_TEXTURE)> m_textureArray{};
    /// @brief A cached material key.
    mutable Maybe<MaterialKey> m_materialKey = Nothing;
};

} // namespace siren::core

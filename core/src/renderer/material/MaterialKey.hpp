#pragma once

namespace siren::renderer
{

/**
 * @brief Enum listing all possible textures that can be uploaded to the GPU
 */
enum class TextureType : size_t {
    BASE_COLOR = 0,
    METALLIC,
    ROUGHNESS,
    EMISSION,
    OCCLUSION,
    NORMAL,

    MAX_TEXTURE, // amount of texture types
};

/**
 * @brief Defines what type of rendering this material should be rendered with
 */
enum class ShadingMode {
    LIT, //
    UNLIT,
    PBR
};

/**
 * @brief Is used to identify which uniforms a material requires. Is used by ShaderManager for
 * caching compiled shaders.
 */
struct MaterialKey {
    ShadingMode shadingMode{ ShadingMode::PBR };

    bool operator==(const MaterialKey&) const;
};

} // namespace siren::renderer

// make MaterialKey hashable and usable as a key in hash maps
template <>
struct std::hash<siren::renderer::MaterialKey> {
    size_t operator()(const siren::renderer::MaterialKey& key) const noexcept
    {
        uint64_t bits = 0;
        bits |= static_cast<uint64_t>(key.shadingMode) << 0;
        return ::std::hash<uint64_t>{}(bits);
    }
};

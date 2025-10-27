#pragma once

namespace siren::core
{

/**
 * @brief Defines what type of rendering this material should be rendered with
 */
enum class ShadingMode { LIT, UNLIT, PBR };

/**
 * @brief Is used to identify which uniforms a material requires. Is used by ShaderManager for
 * caching compiled shaders.
 */
struct MaterialKey
{
    ShadingMode shadingMode{ShadingMode::PBR};
    // todo: flesh this out
    bool operator==(const MaterialKey&) const;
};

} // namespace siren::core

// make MaterialKey hashable and usable as a key in hash maps
template <>
struct std::hash<siren::core::MaterialKey>
{
    size_t operator()(const siren::core::MaterialKey& key) const noexcept
    {
        siren::u64 bits = 0;
        bits |= static_cast<siren::u64>(key.shadingMode) << 0;
        return ::std::hash<siren::u64>{}(bits);
    }
};

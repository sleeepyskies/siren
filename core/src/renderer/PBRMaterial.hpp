#pragma once

#include "assets/Asset.hpp"
#include "resources/GraphicsPipeline.hpp"
#include "resources/Texture.hpp"


namespace siren::core
{
enum class AlphaMode;
}


namespace siren::core
{

/**
 * @brief A Basic PBR material that closely follows the GLTF specs.
 */
class PBRMaterial : public Asset {
public:
    PBRMaterial() = default;

    // setters

    auto set_base_color(const Vec4f& value) noexcept -> void;
    auto set_metallic(f32 value) noexcept -> void;
    auto set_roughness(f32 value) noexcept -> void;
    auto set_base_color_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_metallic_roughness_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_clear_coat(f32 value) noexcept -> void;
    auto set_clear_coat_roughness(f32 value) noexcept -> void;
    auto set_clear_coat_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_clear_coat_roughness_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_clearcoat_normal_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_transmission(f32 value) noexcept -> void;
    auto set_transmission_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_thickness(f32 value) noexcept -> void;
    auto set_attenuation_color(const Vec3f& value) noexcept -> void;
    auto set_attenuation_distance(f32 value) noexcept -> void;
    auto set_thickness_texture(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_ior(f32 value) noexcept -> void;
    auto set_specular_factor(f32 value) noexcept -> void;
    auto set_specular_color(const Vec3f& value) noexcept -> void;
    auto set_specular_color_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_specular_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_sheen_color(const Vec3f& value) noexcept -> void;
    auto set_sheen_roughness(f32 value) noexcept -> void;
    auto set_sheen_color_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_sheen_roughness_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_emissive_strength(f32 value) noexcept -> void;
    auto set_emissive_color(const Vec3f& value) noexcept -> void;
    auto set_emissive_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_iridescence_factor(f32 value) noexcept -> void;
    auto set_iridescence_ior(f32 value) noexcept -> void;
    auto set_iridescence_min(f32 value) noexcept -> void;
    auto set_iridescence_max(f32 value) noexcept -> void;
    auto set_iridescence_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_iridescence_thickness_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_diffuse_transmission_factor(f32 value) noexcept -> void;
    auto set_diffuse_transmission_color(const Vec3f& value) noexcept -> void;
    auto set_diffuse_transmission_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_diffuse_transmission_color_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_anisotropy_strength(f32 value) noexcept -> void;
    auto set_anisotropy_rotation(f32 value) noexcept -> void;
    auto set_anisotropy_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_dispersion(f32 value) noexcept -> void;

    auto set_normal_tex(const StrongHandle<Texture>& handle) noexcept -> void;
    auto set_occlusion_tex(const StrongHandle<Texture>& handle) noexcept -> void;

    auto set_alpha_mode(AlphaMode value) noexcept -> void;
    auto set_alpha_cutoff(f32 value) noexcept -> void;
    auto set_double_sided(bool value) noexcept -> void;
    auto set_unlit(bool value) noexcept -> void;

    // getters

    auto base_color() const noexcept -> const Vec4f&;
    auto metallic() const noexcept -> f32;
    auto roughness() const noexcept -> f32;
    auto base_color_tex() const noexcept -> const StrongHandle<Texture>&;
    auto metallic_roughness_tex() const noexcept -> const StrongHandle<Texture>&;

    auto clear_coat() const noexcept -> f32;
    auto clear_coat_roughness() const noexcept -> f32;
    auto clear_coat_tex() const noexcept -> const StrongHandle<Texture>&;
    auto clear_coat_roughness_tex() const noexcept -> const StrongHandle<Texture>&;
    auto clearcoat_normal_tex() const noexcept -> const StrongHandle<Texture>&;

    auto transmission() const noexcept -> f32;
    auto transmission_tex() const noexcept -> const StrongHandle<Texture>&;
    auto thickness() const noexcept -> f32;
    auto attenuation_color() const noexcept -> const Vec3f&;
    auto attenuation_distance() const noexcept -> f32;
    auto thickness_texture() const noexcept -> const StrongHandle<Texture>&;

    auto ior() const noexcept -> f32;
    auto specular_factor() const noexcept -> f32;
    auto specular_color() const noexcept -> const Vec3f&;
    auto specular_color_tex() const noexcept -> const StrongHandle<Texture>&;
    auto specular_tex() const noexcept -> const StrongHandle<Texture>&;

    auto sheen_color() const noexcept -> const Vec3f&;
    auto sheen_roughness() const noexcept -> f32;
    auto sheen_color_tex() const noexcept -> const StrongHandle<Texture>&;
    auto sheen_roughness_tex() const noexcept -> const StrongHandle<Texture>&;

    auto emissive_strength() const noexcept -> f32;
    auto emissive_color() const noexcept -> const Vec3f&;
    auto emissive_tex() const noexcept -> const StrongHandle<Texture>&;

    auto iridescence_factor() const noexcept -> f32;
    auto iridescence_ior() const noexcept -> f32;
    auto iridescence_min() const noexcept -> f32;
    auto iridescence_max() const noexcept -> f32;
    auto iridescence_tex() const noexcept -> const StrongHandle<Texture>&;
    auto iridescence_thickness_tex() const noexcept -> const StrongHandle<Texture>&;

    auto diffuse_transmission_factor() const noexcept -> f32;
    auto diffuse_transmission_color() const noexcept -> const Vec3f&;
    auto diffuse_transmission_tex() const noexcept -> const StrongHandle<Texture>&;
    auto diffuse_transmission_color_tex() const noexcept -> const StrongHandle<Texture>&;

    auto anisotropy_strength() const noexcept -> f32;
    auto anisotropy_rotation() const noexcept -> f32;
    auto anisotropy_tex() const noexcept -> const StrongHandle<Texture>&;
    auto dispersion() const noexcept -> f32;

    auto normal_tex() const noexcept -> const StrongHandle<Texture>&;
    auto occlusion_tex() const noexcept -> const StrongHandle<Texture>&;

    auto alpha_mode() const noexcept -> AlphaMode;
    auto alpha_cutoff() const noexcept -> f32;
    auto double_sided() const noexcept -> bool;
    auto unlit() const noexcept -> bool;

private:
    Vec4f m_base_color{ 1.0f };
    f32 m_metallic  = 1.0f;
    f32 m_roughness = 1.0f;
    StrongHandle<Texture> m_base_color_tex;
    StrongHandle<Texture> m_metallic_roughness_tex;

    f32 m_clear_coat           = 0.0f;
    f32 m_clear_coat_roughness = 0.0f;
    StrongHandle<Texture> m_clear_coat_tex;
    StrongHandle<Texture> m_clear_coat_roughness_tex;
    StrongHandle<Texture> m_clearcoat_normal_tex;

    f32 m_transmission = 0.0f;
    StrongHandle<Texture> m_transmission_tex;
    f32 m_thickness = 0.0f;
    Vec3f m_attenuation_color{ 1.f };
    f32 m_attenuation_distance = 0.0f;
    StrongHandle<Texture> m_thickness_texture;

    f32 m_ior             = 1.5f;
    f32 m_specular_factor = 1.0f;
    Vec3f m_specular_color{ 1.f };
    StrongHandle<Texture> m_specular_color_tex;
    StrongHandle<Texture> m_specular_tex;

    Vec3f m_sheen_color{ 0.f };
    f32 m_sheen_roughness = 0.0f;
    StrongHandle<Texture> m_sheen_color_tex;
    StrongHandle<Texture> m_sheen_roughness_tex;

    f32 m_iridescence_factor = 0.0f;
    f32 m_iridescence_ior    = 1.3f;
    f32 m_iridescence_min    = 100.0f;
    f32 m_iridescence_max    = 400.0f;
    StrongHandle<Texture> m_iridescence_tex;
    StrongHandle<Texture> m_iridescence_thickness_tex;

    f32 m_diffuse_transmission_factor = 0.0f;
    Vec3f m_diffuse_transmission_color{ 1.f };
    StrongHandle<Texture> m_diffuse_transmission_tex;
    StrongHandle<Texture> m_diffuse_transmission_color_tex;

    f32 m_anisotropy_strength = 0.0f;
    f32 m_anisotropy_rotation = 0.0f;
    StrongHandle<Texture> m_anisotropy_tex;
    f32 m_dispersion = 0.0f;

    StrongHandle<Texture> m_normal_tex;
    StrongHandle<Texture> m_occlusion_tex;
    StrongHandle<Texture> m_emissive_tex;
    Vec3f m_emissive_color{ 0.f };
    f32 m_emissive_strength = 1.0f;

    AlphaMode m_alpha_mode = AlphaMode::Opaque;
    f32 m_alpha_cutoff     = 0.5f;
    bool m_double_sided    = false;
    bool m_unlit           = false;
};

} // namespace siren::core

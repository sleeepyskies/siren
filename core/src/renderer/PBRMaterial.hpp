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

    void set_base_color(const Vec4f& v) { m_base_color = v; }
    void set_metallic(const float v) { m_metallic = v; }
    void set_roughness(const float v) { m_roughness = v; }
    void set_base_color_tex(const StrongHandle<Texture>& handle) { m_base_color_tex = handle; }
    void set_metallic_roughness_tex(const StrongHandle<Texture>& handle) { m_metallic_roughness_tex = handle; }

    void set_clear_coat(const float v) { m_clear_coat = v; }
    void set_clear_coat_roughness(const float v) { m_clear_coat_roughness = v; }
    void set_clear_coat_tex(const StrongHandle<Texture>& handle) { m_clear_coat_tex = handle; }
    void set_clear_coat_roughness_tex(const StrongHandle<Texture>& handle) { m_clear_coat_roughness_tex = handle; }
    void set_clearcoat_normal_tex(const StrongHandle<Texture>& handle) { m_clearcoat_normal_tex = handle; }

    void set_transmission(const float v) { m_transmission = v; }
    void set_transmission_tex(const StrongHandle<Texture>& handle) { m_transmission_tex = handle; }
    void set_thickness(const float v) { m_thickness = v; }
    void set_attenuation_color(const Vec3f& v) { m_attenuation_color = v; }
    void set_attenuation_distance(const float v) { m_attenuation_distance = v; }
    void set_thickness_texture(const StrongHandle<Texture>& handle) { m_thickness_texture = handle; }

    void set_ior(const float v) { m_ior = v; }
    void set_specular_factor(const float v) { m_specular_factor = v; }
    void set_specular_color(const Vec3f& v) { m_specular_color = v; }
    void set_specular_color_tex(const StrongHandle<Texture>& handle) { m_specular_color_tex = handle; }
    void set_specular_tex(const StrongHandle<Texture>& handle) { m_specular_tex = handle; }

    void set_sheen_color(const Vec3f& v) { m_sheen_color = v; }
    void set_sheen_roughness(const float v) { m_sheen_roughness = v; }
    void set_sheen_color_tex(const StrongHandle<Texture>& handle) { m_sheen_color_tex = handle; }
    void set_sheen_roughness_tex(const StrongHandle<Texture>& handle) { m_sheen_roughness_tex = handle; }

    void set_emissive_strength(const float v) { m_emissive_strength = v; }
    void set_emissive_color(const Vec3f& v) { m_emissive_color = v; }
    void set_emissive_tex(const StrongHandle<Texture>& handle) { m_emissive_tex = handle; }

    void set_iridescence_factor(const float v) { m_iridescence_factor = v; }
    void set_iridescence_ior(const float v) { m_iridescence_ior = v; }
    void set_iridescence_min(const float v) { m_iridescence_min = v; }
    void set_iridescence_max(const float v) { m_iridescence_max = v; }
    void set_iridescence_tex(const StrongHandle<Texture>& handle) { m_iridescence_tex = handle; }
    void set_iridescence_thickness_tex(const StrongHandle<Texture>& handle) { m_iridescence_thickness_tex = handle; }

    void set_diffuse_transmission_factor(const float v) { m_diffuse_transmission_factor = v; }
    void set_diffuse_transmission_color(const Vec3f& v) { m_diffuse_transmission_color = v; }
    void set_diffuse_transmission_tex(const StrongHandle<Texture>& handle) { m_diffuse_transmission_tex = handle; }
    void set_diffuse_transmission_color_tex(const StrongHandle<Texture>& handle) {
        m_diffuse_transmission_color_tex = handle;
    }

    void set_anisotropy_strength(const float v) { m_anisotropy_strength = v; }
    void set_anisotropy_rotation(const float v) { m_anisotropy_rotation = v; }
    void set_anisotropy_tex(const StrongHandle<Texture>& handle) { m_anisotropy_tex = handle; }
    void set_dispersion(const float v) { m_dispersion = v; }

    void set_normal_tex(const StrongHandle<Texture>& handle) { m_normal_tex = handle; }
    void set_occlusion_tex(const StrongHandle<Texture>& handle) { m_occlusion_tex = handle; }

    void set_alpha_mode(const AlphaMode mode) { m_alpha_mode = mode; }
    void set_alpha_cutoff(const float v) { m_alpha_cutoff = v; }
    void set_double_sided(const bool b) { m_double_sided = b; }
    void set_unlit(const bool b) { m_unlit = b; }

    const Vec4f& get_base_color() const { return m_base_color; }
    float get_metallic() const { return m_metallic; }
    float get_roughness() const { return m_roughness; }
    StrongHandle<Texture> get_base_color_tex() const { return m_base_color_tex; }
    StrongHandle<Texture> get_metallic_roughness_tex() const { return m_metallic_roughness_tex; }

    float get_clear_coat() const { return m_clear_coat; }
    float get_clear_coat_roughness() const { return m_clear_coat_roughness; }
    StrongHandle<Texture> get_normal_tex() const { return m_normal_tex; }
    const Vec3f& get_emissive_color() const { return m_emissive_color; }
    float get_emissive_strength() const { return m_emissive_strength; }

    AlphaMode get_alpha_mode() const { return m_alpha_mode; }
    float get_alpha_cutoff() const { return m_alpha_cutoff; }
    bool is_double_sided() const { return m_double_sided; }
    bool is_unlit() const { return m_unlit; }

private:
    // Main stuffs
    Vec4f m_base_color{ 1.0f };
    float m_metallic  = 1.0f;
    float m_roughness = 1.0f;
    StrongHandle<Texture> m_base_color_tex;
    StrongHandle<Texture> m_metallic_roughness_tex;

    // Clearcoat
    float m_clear_coat           = 0.0f;
    float m_clear_coat_roughness = 0.0f;
    StrongHandle<Texture> m_clear_coat_tex;
    StrongHandle<Texture> m_clear_coat_roughness_tex;
    StrongHandle<Texture> m_clearcoat_normal_tex;

    // Transmission / Volume
    float m_transmission = 0.0f;
    StrongHandle<Texture> m_transmission_tex;
    float m_thickness = 0.0f;
    Vec3f m_attenuation_color{ 1.f };
    float m_attenuation_distance = 0.0f;
    StrongHandle<Texture> m_thickness_texture;

    // Specular / Ior
    float m_ior             = 1.5f;
    float m_specular_factor = 1.0f;
    Vec3f m_specular_color{ 1.f };
    StrongHandle<Texture> m_specular_color_tex;
    StrongHandle<Texture> m_specular_tex;

    // Sheen
    Vec3f m_sheen_color{ 0.f };
    float m_sheen_roughness = 0.0f;
    StrongHandle<Texture> m_sheen_color_tex;
    StrongHandle<Texture> m_sheen_roughness_tex;

    // Iridescence
    float m_iridescence_factor = 0.0f;
    float m_iridescence_ior    = 1.3f;
    float m_iridescence_min    = 100.0f;
    float m_iridescence_max    = 400.0f;
    StrongHandle<Texture> m_iridescence_tex;
    StrongHandle<Texture> m_iridescence_thickness_tex;

    // Diffuse Transmission
    float m_diffuse_transmission_factor = 0.0f;
    Vec3f m_diffuse_transmission_color{ 1.f };
    StrongHandle<Texture> m_diffuse_transmission_tex;
    StrongHandle<Texture> m_diffuse_transmission_color_tex;

    // Anisotropy / Dispersion
    float m_anisotropy_strength = 0.0f;
    float m_anisotropy_rotation = 0.0f;
    StrongHandle<Texture> m_anisotropy_tex;
    float m_dispersion = 0.0f;

    // General
    StrongHandle<Texture> m_normal_tex;
    StrongHandle<Texture> m_occlusion_tex;
    StrongHandle<Texture> m_emissive_tex;
    Vec3f m_emissive_color{ 0.f };
    float m_emissive_strength = 1.0f;

    // Pipeline State
    AlphaMode m_alpha_mode = AlphaMode::Opaque;
    float m_alpha_cutoff   = 0.5f;
    bool m_double_sided    = false;
    bool m_unlit           = false;
};

} // namespace siren::core

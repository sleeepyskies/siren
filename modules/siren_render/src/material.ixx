module;

#include <string>

export module siren.render.material;

import siren.render.texture;
import siren.asset.asset;
import siren.asset.asset_handle;
import siren.common;
import siren.color;
import siren.math;

namespace siren::render {

/**
 * @brief A Basic PBR material that closely follows the GLTF specs.
 */
export class PBRMaterial : public asset::Asset {
public:
    explicit PBRMaterial(const std::string& name) : m_name(name) { }

    // setters

    auto set_base_color(const RGBA& value) noexcept -> void;
    auto set_metallic(f32 value) noexcept -> void;
    auto set_roughness(f32 value) noexcept -> void;
    auto set_base_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_metallic_roughness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_clear_coat(f32 value) noexcept -> void;
    auto set_clear_coat_roughness(f32 value) noexcept -> void;
    auto set_clear_coat_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_clear_coat_roughness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_clearcoat_normal_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_transmission(f32 value) noexcept -> void;
    auto set_transmission_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_thickness(f32 value) noexcept -> void;
    auto set_attenuation_color(const glm::vec3& value) noexcept -> void;
    auto set_attenuation_distance(f32 value) noexcept -> void;
    auto set_thickness_texture(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_ior(f32 value) noexcept -> void;
    auto set_specular_factor(f32 value) noexcept -> void;
    auto set_specular_color(const glm::vec3& value) noexcept -> void;
    auto set_specular_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_specular_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_sheen_color(const glm::vec3& value) noexcept -> void;
    auto set_sheen_roughness(f32 value) noexcept -> void;
    auto set_sheen_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_sheen_roughness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_emissive_strength(f32 value) noexcept -> void;
    auto set_emissive_color(const glm::vec3& value) noexcept -> void;
    auto set_emissive_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_iridescence_factor(f32 value) noexcept -> void;
    auto set_iridescence_ior(f32 value) noexcept -> void;
    auto set_iridescence_min(f32 value) noexcept -> void;
    auto set_iridescence_max(f32 value) noexcept -> void;
    auto set_iridescence_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_iridescence_thickness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_diffuse_transmission_factor(f32 value) noexcept -> void;
    auto set_diffuse_transmission_color(const glm::vec3& value) noexcept -> void;
    auto set_diffuse_transmission_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_diffuse_transmission_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_anisotropy_strength(f32 value) noexcept -> void;
    auto set_anisotropy_rotation(f32 value) noexcept -> void;
    auto set_anisotropy_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_dispersion(f32 value) noexcept -> void;

    auto set_normal_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;
    auto set_occlusion_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void;

    auto set_alpha_mode(AlphaMode value) noexcept -> void;
    auto set_alpha_cutoff(f32 value) noexcept -> void;
    auto set_double_sided(bool value) noexcept -> void;
    auto set_unlit(bool value) noexcept -> void;

    // getters

    [[nodiscard]] auto base_color() const noexcept -> const RGBA&;
    [[nodiscard]] auto metallic() const noexcept -> f32;
    [[nodiscard]] auto roughness() const noexcept -> f32;
    [[nodiscard]] auto base_color_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto metallic_roughness_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto clear_coat() const noexcept -> f32;
    [[nodiscard]] auto clear_coat_roughness() const noexcept -> f32;
    [[nodiscard]] auto clear_coat_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto clear_coat_roughness_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto clearcoat_normal_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto transmission() const noexcept -> f32;
    [[nodiscard]] auto transmission_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto thickness() const noexcept -> f32;
    [[nodiscard]] auto attenuation_color() const noexcept -> const glm::vec3&;
    [[nodiscard]] auto attenuation_distance() const noexcept -> f32;
    [[nodiscard]] auto thickness_texture() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto ior() const noexcept -> f32;
    [[nodiscard]] auto specular_factor() const noexcept -> f32;
    [[nodiscard]] auto specular_color() const noexcept -> const glm::vec3&;
    [[nodiscard]] auto specular_color_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto specular_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto sheen_color() const noexcept -> const glm::vec3&;
    [[nodiscard]] auto sheen_roughness() const noexcept -> f32;
    [[nodiscard]] auto sheen_color_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto sheen_roughness_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto emissive_strength() const noexcept -> f32;
    [[nodiscard]] auto emissive_color() const noexcept -> const glm::vec3&;
    [[nodiscard]] auto emissive_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto iridescence_factor() const noexcept -> f32;
    [[nodiscard]] auto iridescence_ior() const noexcept -> f32;
    [[nodiscard]] auto iridescence_min() const noexcept -> f32;
    [[nodiscard]] auto iridescence_max() const noexcept -> f32;
    [[nodiscard]] auto iridescence_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto iridescence_thickness_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto diffuse_transmission_factor() const noexcept -> f32;
    [[nodiscard]] auto diffuse_transmission_color() const noexcept -> const glm::vec3&;
    [[nodiscard]] auto diffuse_transmission_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto diffuse_transmission_color_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto anisotropy_strength() const noexcept -> f32;
    [[nodiscard]] auto anisotropy_rotation() const noexcept -> f32;
    [[nodiscard]] auto anisotropy_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto dispersion() const noexcept -> f32;

    [[nodiscard]] auto normal_tex() const noexcept -> const asset::StrongHandle<Texture>&;
    [[nodiscard]] auto occlusion_tex() const noexcept -> const asset::StrongHandle<Texture>&;

    [[nodiscard]] auto alpha_mode() const noexcept -> AlphaMode;
    [[nodiscard]] auto alpha_cutoff() const noexcept -> f32;
    [[nodiscard]] auto double_sided() const noexcept -> bool;
    [[nodiscard]] auto unlit() const noexcept -> bool;

private:
    std::string m_name;

    glm::vec4 m_base_color{ 1.0f };
    f32 m_metallic  = 1.0f;
    f32 m_roughness = 1.0f;
    asset::StrongHandle<Texture> m_base_color_tex;
    asset::StrongHandle<Texture> m_metallic_roughness_tex;

    f32 m_clear_coat           = 0.0f;
    f32 m_clear_coat_roughness = 0.0f;
    asset::StrongHandle<Texture> m_clear_coat_tex;
    asset::StrongHandle<Texture> m_clear_coat_roughness_tex;
    asset::StrongHandle<Texture> m_clearcoat_normal_tex;

    f32 m_transmission = 0.0f;
    asset::StrongHandle<Texture> m_transmission_tex;
    f32 m_thickness = 0.0f;
    glm::vec3 m_attenuation_color{ 1.f };
    f32 m_attenuation_distance = 0.0f;
    asset::StrongHandle<Texture> m_thickness_texture;

    f32 m_ior             = 1.5f;
    f32 m_specular_factor = 1.0f;
    glm::vec3 m_specular_color{ 1.f };
    asset::StrongHandle<Texture> m_specular_color_tex;
    asset::StrongHandle<Texture> m_specular_tex;

    glm::vec3 m_sheen_color{ 0.f };
    f32 m_sheen_roughness = 0.0f;
    asset::StrongHandle<Texture> m_sheen_color_tex;
    asset::StrongHandle<Texture> m_sheen_roughness_tex;

    f32 m_iridescence_factor = 0.0f;
    f32 m_iridescence_ior    = 1.3f;
    f32 m_iridescence_min    = 100.0f;
    f32 m_iridescence_max    = 400.0f;
    asset::StrongHandle<Texture> m_iridescence_tex;
    asset::StrongHandle<Texture> m_iridescence_thickness_tex;

    f32 m_diffuse_transmission_factor = 0.0f;
    glm::vec3 m_diffuse_transmission_color{ 1.f };
    asset::StrongHandle<Texture> m_diffuse_transmission_tex;
    asset::StrongHandle<Texture> m_diffuse_transmission_color_tex;

    f32 m_anisotropy_strength = 0.0f;
    f32 m_anisotropy_rotation = 0.0f;
    asset::StrongHandle<Texture> m_anisotropy_tex;
    f32 m_dispersion = 0.0f;

    asset::StrongHandle<Texture> m_normal_tex;
    asset::StrongHandle<Texture> m_occlusion_tex;
    asset::StrongHandle<Texture> m_emissive_tex;
    glm::vec3 m_emissive_color{ 0.f };
    f32 m_emissive_strength = 1.0f;

    AlphaMode m_alpha_mode = AlphaMode::Opaque;
    f32 m_alpha_cutoff     = 0.5f;
    bool m_double_sided    = false;
    bool m_unlit           = false;
};

auto PBRMaterial::set_base_color(const RGBA& value) noexcept -> void { m_base_color = value; }
auto PBRMaterial::set_metallic(const f32 value) noexcept -> void { m_metallic = value; }
auto PBRMaterial::set_roughness(const f32 value) noexcept -> void { m_roughness = value; }
auto PBRMaterial::set_base_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_base_color_tex = handle;
}
auto PBRMaterial::set_metallic_roughness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_metallic_roughness_tex = handle;
}

auto PBRMaterial::set_clear_coat(const f32 value) noexcept -> void { m_clear_coat = value; }
auto PBRMaterial::set_clear_coat_roughness(const f32 value) noexcept -> void { m_clear_coat_roughness = value; }
auto PBRMaterial::set_clear_coat_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_clear_coat_tex = handle;
}
auto PBRMaterial::set_clear_coat_roughness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_clear_coat_roughness_tex = handle;
}
auto PBRMaterial::set_clearcoat_normal_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_clearcoat_normal_tex = handle;
}

auto PBRMaterial::set_transmission(const f32 value) noexcept -> void { m_transmission = value; }
auto PBRMaterial::set_transmission_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_transmission_tex = handle;
}
auto PBRMaterial::set_thickness(const f32 value) noexcept -> void { m_thickness = value; }
auto PBRMaterial::set_attenuation_color(const glm::vec3& value) noexcept -> void { m_attenuation_color = value; }
auto PBRMaterial::set_attenuation_distance(const f32 value) noexcept -> void { m_attenuation_distance = value; }
auto PBRMaterial::set_thickness_texture(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_thickness_texture = handle;
}

auto PBRMaterial::set_ior(const f32 value) noexcept -> void { m_ior = value; }
auto PBRMaterial::set_specular_factor(const f32 value) noexcept -> void { m_specular_factor = value; }
auto PBRMaterial::set_specular_color(const glm::vec3& value) noexcept -> void { m_specular_color = value; }
auto PBRMaterial::set_specular_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_specular_color_tex = handle;
}
auto PBRMaterial::set_specular_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_specular_tex = handle;
}

auto PBRMaterial::set_sheen_color(const glm::vec3& value) noexcept -> void { m_sheen_color = value; }
auto PBRMaterial::set_sheen_roughness(const f32 value) noexcept -> void { m_sheen_roughness = value; }
auto PBRMaterial::set_sheen_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_sheen_color_tex = handle;
}
auto PBRMaterial::set_sheen_roughness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_sheen_roughness_tex = handle;
}

auto PBRMaterial::set_emissive_strength(const f32 value) noexcept -> void { m_emissive_strength = value; }
auto PBRMaterial::set_emissive_color(const glm::vec3& value) noexcept -> void { m_emissive_color = value; }
auto PBRMaterial::set_emissive_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_emissive_tex = handle;
}

auto PBRMaterial::set_iridescence_factor(const f32 value) noexcept -> void { m_iridescence_factor = value; }
auto PBRMaterial::set_iridescence_ior(const f32 value) noexcept -> void { m_iridescence_ior = value; }
auto PBRMaterial::set_iridescence_min(const f32 value) noexcept -> void { m_iridescence_min = value; }
auto PBRMaterial::set_iridescence_max(const f32 value) noexcept -> void { m_iridescence_max = value; }
auto PBRMaterial::set_iridescence_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_iridescence_tex = handle;
}
auto PBRMaterial::set_iridescence_thickness_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_iridescence_thickness_tex = handle;
}

auto PBRMaterial::set_diffuse_transmission_factor(const f32 value) noexcept -> void {
    m_diffuse_transmission_factor = value;
}
auto PBRMaterial::set_diffuse_transmission_color(const glm::vec3& value) noexcept -> void {
    m_diffuse_transmission_color = value;
}
auto PBRMaterial::set_diffuse_transmission_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_diffuse_transmission_tex = handle;
}
auto PBRMaterial::set_diffuse_transmission_color_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_diffuse_transmission_color_tex = handle;
}

auto PBRMaterial::set_anisotropy_strength(const f32 value) noexcept -> void { m_anisotropy_strength = value; }
auto PBRMaterial::set_anisotropy_rotation(const f32 value) noexcept -> void { m_anisotropy_rotation = value; }
auto PBRMaterial::set_anisotropy_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_anisotropy_tex = handle;
}
auto PBRMaterial::set_dispersion(const f32 value) noexcept -> void { m_dispersion = value; }

auto PBRMaterial::set_normal_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void { m_normal_tex = handle; }
auto PBRMaterial::set_occlusion_tex(const asset::StrongHandle<Texture>& handle) noexcept -> void {
    m_occlusion_tex = handle;
}

auto PBRMaterial::set_alpha_mode(const AlphaMode value) noexcept -> void { m_alpha_mode = value; }
auto PBRMaterial::set_alpha_cutoff(const f32 value) noexcept -> void { m_alpha_cutoff = value; }
auto PBRMaterial::set_double_sided(const bool value) noexcept -> void { m_double_sided = value; }
auto PBRMaterial::set_unlit(const bool value) noexcept -> void { m_unlit = value; }

// getters

auto PBRMaterial::base_color() const noexcept -> const glm::vec4& { return m_base_color; }
auto PBRMaterial::metallic() const noexcept -> f32 { return m_metallic; }
auto PBRMaterial::roughness() const noexcept -> f32 { return m_roughness; }
auto PBRMaterial::base_color_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_base_color_tex; }
auto PBRMaterial::metallic_roughness_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_metallic_roughness_tex;
}

auto PBRMaterial::clear_coat() const noexcept -> f32 { return m_clear_coat; }
auto PBRMaterial::clear_coat_roughness() const noexcept -> f32 { return m_clear_coat_roughness; }
auto PBRMaterial::clear_coat_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_clear_coat_tex; }
auto PBRMaterial::clear_coat_roughness_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_clear_coat_roughness_tex;
}
auto PBRMaterial::clearcoat_normal_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_clearcoat_normal_tex;
}

auto PBRMaterial::transmission() const noexcept -> f32 { return m_transmission; }
auto PBRMaterial::transmission_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_transmission_tex;
}
auto PBRMaterial::thickness() const noexcept -> f32 { return m_thickness; }
auto PBRMaterial::attenuation_color() const noexcept -> const glm::vec3& { return m_attenuation_color; }
auto PBRMaterial::attenuation_distance() const noexcept -> f32 { return m_attenuation_distance; }
auto PBRMaterial::thickness_texture() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_thickness_texture;
}

auto PBRMaterial::ior() const noexcept -> f32 { return m_ior; }
auto PBRMaterial::specular_factor() const noexcept -> f32 { return m_specular_factor; }
auto PBRMaterial::specular_color() const noexcept -> const glm::vec3& { return m_specular_color; }
auto PBRMaterial::specular_color_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_specular_color_tex;
}
auto PBRMaterial::specular_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_specular_tex; }

auto PBRMaterial::sheen_color() const noexcept -> const glm::vec3& { return m_sheen_color; }
auto PBRMaterial::sheen_roughness() const noexcept -> f32 { return m_sheen_roughness; }
auto PBRMaterial::sheen_color_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_sheen_color_tex; }
auto PBRMaterial::sheen_roughness_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_sheen_roughness_tex;
}

auto PBRMaterial::emissive_strength() const noexcept -> f32 { return m_emissive_strength; }
auto PBRMaterial::emissive_color() const noexcept -> const glm::vec3& { return m_emissive_color; }
auto PBRMaterial::emissive_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_emissive_tex; }

auto PBRMaterial::iridescence_factor() const noexcept -> f32 { return m_iridescence_factor; }
auto PBRMaterial::iridescence_ior() const noexcept -> f32 { return m_iridescence_ior; }
auto PBRMaterial::iridescence_min() const noexcept -> f32 { return m_iridescence_min; }
auto PBRMaterial::iridescence_max() const noexcept -> f32 { return m_iridescence_max; }
auto PBRMaterial::iridescence_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_iridescence_tex; }
auto PBRMaterial::iridescence_thickness_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_iridescence_thickness_tex;
}

auto PBRMaterial::diffuse_transmission_factor() const noexcept -> f32 { return m_diffuse_transmission_factor; }
auto PBRMaterial::diffuse_transmission_color() const noexcept -> const glm::vec3& {
    return m_diffuse_transmission_color;
}
auto PBRMaterial::diffuse_transmission_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_diffuse_transmission_tex;
}
auto PBRMaterial::diffuse_transmission_color_tex() const noexcept -> const asset::StrongHandle<Texture>& {
    return m_diffuse_transmission_color_tex;
}

auto PBRMaterial::anisotropy_strength() const noexcept -> f32 { return m_anisotropy_strength; }
auto PBRMaterial::anisotropy_rotation() const noexcept -> f32 { return m_anisotropy_rotation; }
auto PBRMaterial::anisotropy_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_anisotropy_tex; }
auto PBRMaterial::dispersion() const noexcept -> f32 { return m_dispersion; }

auto PBRMaterial::normal_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_normal_tex; }
auto PBRMaterial::occlusion_tex() const noexcept -> const asset::StrongHandle<Texture>& { return m_occlusion_tex; }

auto PBRMaterial::alpha_mode() const noexcept -> AlphaMode { return m_alpha_mode; }
auto PBRMaterial::alpha_cutoff() const noexcept -> f32 { return m_alpha_cutoff; }
auto PBRMaterial::double_sided() const noexcept -> bool { return m_double_sided; }
auto PBRMaterial::unlit() const noexcept -> bool { return m_unlit; }

} // namespace siren::render

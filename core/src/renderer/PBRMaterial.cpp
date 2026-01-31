#include "PBRMaterial.hpp"


namespace siren::core
{
// setters

auto PBRMaterial::set_base_color(const Vec4f& value) noexcept -> void { m_base_color = value; }
auto PBRMaterial::set_metallic(const f32 value) noexcept -> void { m_metallic = value; }
auto PBRMaterial::set_roughness(const f32 value) noexcept -> void { m_roughness = value; }
auto PBRMaterial::set_base_color_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_base_color_tex = handle;
}
auto PBRMaterial::set_metallic_roughness_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_metallic_roughness_tex = handle;
}

auto PBRMaterial::set_clear_coat(const f32 value) noexcept -> void { m_clear_coat = value; }
auto PBRMaterial::set_clear_coat_roughness(const f32 value) noexcept -> void { m_clear_coat_roughness = value; }
auto PBRMaterial::set_clear_coat_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_clear_coat_tex = handle;
}
auto PBRMaterial::set_clear_coat_roughness_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_clear_coat_roughness_tex = handle;
}
auto PBRMaterial::set_clearcoat_normal_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_clearcoat_normal_tex = handle;
}

auto PBRMaterial::set_transmission(const f32 value) noexcept -> void { m_transmission = value; }
auto PBRMaterial::set_transmission_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_transmission_tex = handle;
}
auto PBRMaterial::set_thickness(const f32 value) noexcept -> void { m_thickness = value; }
auto PBRMaterial::set_attenuation_color(const Vec3f& value) noexcept -> void { m_attenuation_color = value; }
auto PBRMaterial::set_attenuation_distance(const f32 value) noexcept -> void { m_attenuation_distance = value; }
auto PBRMaterial::set_thickness_texture(const StrongHandle<Texture>& handle) noexcept -> void {
    m_thickness_texture = handle;
}

auto PBRMaterial::set_ior(const f32 value) noexcept -> void { m_ior = value; }
auto PBRMaterial::set_specular_factor(const f32 value) noexcept -> void { m_specular_factor = value; }
auto PBRMaterial::set_specular_color(const Vec3f& value) noexcept -> void { m_specular_color = value; }
auto PBRMaterial::set_specular_color_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_specular_color_tex = handle;
}
auto PBRMaterial::set_specular_tex(const StrongHandle<Texture>& handle) noexcept -> void { m_specular_tex = handle; }

auto PBRMaterial::set_sheen_color(const Vec3f& value) noexcept -> void { m_sheen_color = value; }
auto PBRMaterial::set_sheen_roughness(const f32 value) noexcept -> void { m_sheen_roughness = value; }
auto PBRMaterial::set_sheen_color_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_sheen_color_tex = handle;
}
auto PBRMaterial::set_sheen_roughness_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_sheen_roughness_tex = handle;
}

auto PBRMaterial::set_emissive_strength(const f32 value) noexcept -> void { m_emissive_strength = value; }
auto PBRMaterial::set_emissive_color(const Vec3f& value) noexcept -> void { m_emissive_color = value; }
auto PBRMaterial::set_emissive_tex(const StrongHandle<Texture>& handle) noexcept -> void { m_emissive_tex = handle; }

auto PBRMaterial::set_iridescence_factor(const f32 value) noexcept -> void { m_iridescence_factor = value; }
auto PBRMaterial::set_iridescence_ior(const f32 value) noexcept -> void { m_iridescence_ior = value; }
auto PBRMaterial::set_iridescence_min(const f32 value) noexcept -> void { m_iridescence_min = value; }
auto PBRMaterial::set_iridescence_max(const f32 value) noexcept -> void { m_iridescence_max = value; }
auto PBRMaterial::set_iridescence_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_iridescence_tex = handle;
}
auto PBRMaterial::set_iridescence_thickness_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_iridescence_thickness_tex = handle;
}

auto PBRMaterial::set_diffuse_transmission_factor(const f32 value) noexcept -> void {
    m_diffuse_transmission_factor = value;
}
auto PBRMaterial::set_diffuse_transmission_color(const Vec3f& value) noexcept -> void {
    m_diffuse_transmission_color = value;
}
auto PBRMaterial::set_diffuse_transmission_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_diffuse_transmission_tex = handle;
}
auto PBRMaterial::set_diffuse_transmission_color_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_diffuse_transmission_color_tex = handle;
}

auto PBRMaterial::set_anisotropy_strength(const f32 value) noexcept -> void { m_anisotropy_strength = value; }
auto PBRMaterial::set_anisotropy_rotation(const f32 value) noexcept -> void { m_anisotropy_rotation = value; }
auto PBRMaterial::set_anisotropy_tex(const StrongHandle<Texture>& handle) noexcept -> void {
    m_anisotropy_tex = handle;
}
auto PBRMaterial::set_dispersion(const f32 value) noexcept -> void { m_dispersion = value; }

auto PBRMaterial::set_normal_tex(const StrongHandle<Texture>& handle) noexcept -> void { m_normal_tex = handle; }
auto PBRMaterial::set_occlusion_tex(const StrongHandle<Texture>& handle) noexcept -> void { m_occlusion_tex = handle; }

auto PBRMaterial::set_alpha_mode(const AlphaMode value) noexcept -> void { m_alpha_mode = value; }
auto PBRMaterial::set_alpha_cutoff(const f32 value) noexcept -> void { m_alpha_cutoff = value; }
auto PBRMaterial::set_double_sided(const bool value) noexcept -> void { m_double_sided = value; }
auto PBRMaterial::set_unlit(const bool value) noexcept -> void { m_unlit = value; }

// getters

auto PBRMaterial::base_color() const noexcept -> const Vec4f& { return m_base_color; }
auto PBRMaterial::metallic() const noexcept -> f32 { return m_metallic; }
auto PBRMaterial::roughness() const noexcept -> f32 { return m_roughness; }
auto PBRMaterial::base_color_tex() const noexcept -> const StrongHandle<Texture>& { return m_base_color_tex; }
auto PBRMaterial::metallic_roughness_tex() const noexcept -> const StrongHandle<Texture>& {
    return m_metallic_roughness_tex;
}

auto PBRMaterial::clear_coat() const noexcept -> f32 { return m_clear_coat; }
auto PBRMaterial::clear_coat_roughness() const noexcept -> f32 { return m_clear_coat_roughness; }
auto PBRMaterial::clear_coat_tex() const noexcept -> const StrongHandle<Texture>& { return m_clear_coat_tex; }
auto PBRMaterial::clear_coat_roughness_tex() const noexcept -> const StrongHandle<Texture>& {
    return m_clear_coat_roughness_tex;
}
auto PBRMaterial::clearcoat_normal_tex() const noexcept -> const StrongHandle<Texture>& {
    return m_clearcoat_normal_tex;
}

auto PBRMaterial::transmission() const noexcept -> f32 { return m_transmission; }
auto PBRMaterial::transmission_tex() const noexcept -> const StrongHandle<Texture>& { return m_transmission_tex; }
auto PBRMaterial::thickness() const noexcept -> f32 { return m_thickness; }
auto PBRMaterial::attenuation_color() const noexcept -> const Vec3f& { return m_attenuation_color; }
auto PBRMaterial::attenuation_distance() const noexcept -> f32 { return m_attenuation_distance; }
auto PBRMaterial::thickness_texture() const noexcept -> const StrongHandle<Texture>& { return m_thickness_texture; }

auto PBRMaterial::ior() const noexcept -> f32 { return m_ior; }
auto PBRMaterial::specular_factor() const noexcept -> f32 { return m_specular_factor; }
auto PBRMaterial::specular_color() const noexcept -> const Vec3f& { return m_specular_color; }
auto PBRMaterial::specular_color_tex() const noexcept -> const StrongHandle<Texture>& { return m_specular_color_tex; }
auto PBRMaterial::specular_tex() const noexcept -> const StrongHandle<Texture>& { return m_specular_tex; }

auto PBRMaterial::sheen_color() const noexcept -> const Vec3f& { return m_sheen_color; }
auto PBRMaterial::sheen_roughness() const noexcept -> f32 { return m_sheen_roughness; }
auto PBRMaterial::sheen_color_tex() const noexcept -> const StrongHandle<Texture>& { return m_sheen_color_tex; }
auto PBRMaterial::sheen_roughness_tex() const noexcept -> const StrongHandle<Texture>& { return m_sheen_roughness_tex; }

auto PBRMaterial::emissive_strength() const noexcept -> f32 { return m_emissive_strength; }
auto PBRMaterial::emissive_color() const noexcept -> const Vec3f& { return m_emissive_color; }
auto PBRMaterial::emissive_tex() const noexcept -> const StrongHandle<Texture>& { return m_emissive_tex; }

auto PBRMaterial::iridescence_factor() const noexcept -> f32 { return m_iridescence_factor; }
auto PBRMaterial::iridescence_ior() const noexcept -> f32 { return m_iridescence_ior; }
auto PBRMaterial::iridescence_min() const noexcept -> f32 { return m_iridescence_min; }
auto PBRMaterial::iridescence_max() const noexcept -> f32 { return m_iridescence_max; }
auto PBRMaterial::iridescence_tex() const noexcept -> const StrongHandle<Texture>& { return m_iridescence_tex; }
auto PBRMaterial::iridescence_thickness_tex() const noexcept -> const StrongHandle<Texture>& {
    return m_iridescence_thickness_tex;
}

auto PBRMaterial::diffuse_transmission_factor() const noexcept -> f32 { return m_diffuse_transmission_factor; }
auto PBRMaterial::diffuse_transmission_color() const noexcept -> const Vec3f& { return m_diffuse_transmission_color; }
auto PBRMaterial::diffuse_transmission_tex() const noexcept -> const StrongHandle<Texture>& {
    return m_diffuse_transmission_tex;
}
auto PBRMaterial::diffuse_transmission_color_tex() const noexcept -> const StrongHandle<Texture>& {
    return m_diffuse_transmission_color_tex;
}

auto PBRMaterial::anisotropy_strength() const noexcept -> f32 { return m_anisotropy_strength; }
auto PBRMaterial::anisotropy_rotation() const noexcept -> f32 { return m_anisotropy_rotation; }
auto PBRMaterial::anisotropy_tex() const noexcept -> const StrongHandle<Texture>& { return m_anisotropy_tex; }
auto PBRMaterial::dispersion() const noexcept -> f32 { return m_dispersion; }

auto PBRMaterial::normal_tex() const noexcept -> const StrongHandle<Texture>& { return m_normal_tex; }
auto PBRMaterial::occlusion_tex() const noexcept -> const StrongHandle<Texture>& { return m_occlusion_tex; }

auto PBRMaterial::alpha_mode() const noexcept -> AlphaMode { return m_alpha_mode; }
auto PBRMaterial::alpha_cutoff() const noexcept -> f32 { return m_alpha_cutoff; }
auto PBRMaterial::double_sided() const noexcept -> bool { return m_double_sided; }
auto PBRMaterial::unlit() const noexcept -> bool { return m_unlit; }

} // namespace siren::core

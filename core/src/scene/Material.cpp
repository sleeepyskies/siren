#include "Material.hpp"

namespace core
{

void Material::bind(const renderer::Shaders& shaders) const
{
    int slot = 0;

    if (m_ambientMap) {
        m_ambientMap->bind(slot);
        shaders.setUniformImage("uAmbientMap", slot++);
        shaders.setUniformBool("hasAmbientMap", m_ambientMap != nullptr);
    }

    if (m_diffuseMap) {
        m_diffuseMap->bind(slot);
        shaders.setUniformImage("uDiffuseMap", slot++);
        shaders.setUniformBool("hasDiffuseMap", m_diffuseMap != nullptr);
    }

    if (m_specularMap) {
        m_specularMap->bind(slot);
        shaders.setUniformImage("uSpecularMap", slot++);
        shaders.setUniformBool("hasSpecularMap", m_specularMap != nullptr);
    }

    if (m_roughnessMap) {
        m_roughnessMap->bind(slot);
        shaders.setUniformImage("uRoughnessMap", slot++);
        shaders.setUniformBool("hasRoughnessMap", m_roughnessMap != nullptr);
    }

    if (m_alphaMap) {
        m_alphaMap->bind(slot);
        shaders.setUniformImage("uAlphaMap", slot++);
        shaders.setUniformBool("hasAlphaMap", m_alphaMap != nullptr);
    }

    if (m_ambience) shaders.setUniformVec3("uAmbientColor", *m_ambience);
    shaders.setUniformBool("hasAmbientColor", m_ambience.has_value());

    if (m_diffuse) shaders.setUniformVec3("uDiffuseColor", *m_diffuse);
    shaders.setUniformBool("hasDiffuseColor", m_diffuse.has_value());

    if (m_specular) shaders.setUniformVec3("uSpecularColor", *m_specular);
    shaders.setUniformBool("hasSpecularColor", m_specular.has_value());

    if (m_roughness) shaders.setUniformFloat("uRoughness", *m_roughness);
    shaders.setUniformBool("hasRoughnessValue", m_roughness.has_value());

    if (m_alpha) shaders.setUniformFloat("uAlpha", *m_alpha);
    shaders.setUniformBool("hasAlphaValue", m_alpha.has_value());
}

void Material::addAmbientMap(ref<renderer::Texture2D> texture)
{
    m_ambientMap = std::move(texture);
}

void Material::addDiffuseMap(ref<renderer::Texture2D> texture)
{
    m_diffuseMap = std::move(texture);
}

void Material::addSpecularMap(ref<renderer::Texture2D> texture)
{
    m_specularMap = std::move(texture);
}

void Material::addRoughnessMap(ref<renderer::Texture2D> texture)
{
    m_roughnessMap = std::move(texture);
}

void Material::addAlphaMap(ref<renderer::Texture2D> texture)
{
    m_alphaMap = std::move(texture);
}

void Material::addAmbience(glm::vec3 ambience)
{
    m_ambience = ambience;
}

void Material::addDiffuse(glm::vec3 diffuse)
{
    m_diffuse = diffuse;
}

void Material::addSpecular(glm::vec3 specular)
{
    m_specular = specular;
}

void Material::addRoughness(float roughness)
{
    m_roughness = roughness;
}

void Material::addAlpha(float alpha)
{
    m_alpha = alpha;
}

} // namespace core

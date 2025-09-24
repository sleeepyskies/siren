#include "Material.hpp"

void Material::addAmbientMap(uref<Texture2D> texture)
{
    m_ambientMap = std::move(texture);
}

void Material::addDiffuseMap(uref<Texture2D> texture)
{
    m_diffuseMap = std::move(texture);
}

void Material::addSpecularMap(uref<Texture2D> texture)
{
    m_specularMap = std::move(texture);
}

void Material::addRoughnessMap(uref<Texture2D> texture)
{
    m_roughnessMap = std::move(texture);
}

void Material::addAlphaMap(uref<Texture2D> texture)
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
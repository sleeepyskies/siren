#pragma once

#include <Texture.hpp>
#include <glm/vec3.hpp>
#include <types.hpp>

class Material
{
public:
    Material()  = default;
    ~Material() = default;

    void addAmbientMap(uref<Texture2D> texture);
    void addDiffuseMap(uref<Texture2D> texture);
    void addSpecularMap(uref<Texture2D> texture);
    void addRoughnessMap(uref<Texture2D> texture);
    void addAlphaMap(uref<Texture2D> texture);
    void addAmbience(glm::vec3 ambience);
    void addDiffuse(glm::vec3 diffuse);
    void addSpecular(glm::vec3 specular);
    void addRoughness(float roughness);
    void addAlpha(float alpha);

private:
    ref<Texture2D> m_ambientMap         = nullptr;
    ref<Texture2D> m_diffuseMap         = nullptr;
    ref<Texture2D> m_specularMap        = nullptr;
    ref<Texture2D> m_roughnessMap       = nullptr;
    ref<Texture2D> m_alphaMap           = nullptr;
    std::optional<glm::vec3> m_ambience = std::nullopt;
    std::optional<glm::vec3> m_diffuse  = std::nullopt;
    std::optional<glm::vec3> m_specular = std::nullopt;
    std::optional<float> m_roughness    = std::nullopt;
    std::optional<float> m_alpha        = std::nullopt;
};

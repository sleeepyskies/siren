#pragma once

#include "renderer/Shaders.hpp"
#include "renderer/Texture.hpp"
#include "utilities/spch.hpp"

namespace core
{

class Material
{
public:
    explicit Material(const std::string& name) : m_name(name)
    {
    }
    ~Material() = default;

    void bind(const renderer::Shaders& shaders) const;

    void addAmbientMap(ref<renderer::Texture2D> texture);
    void addDiffuseMap(ref<renderer::Texture2D> texture);
    void addSpecularMap(ref<renderer::Texture2D> texture);
    void addRoughnessMap(ref<renderer::Texture2D> texture);
    void addAlphaMap(ref<renderer::Texture2D> texture);
    void addAmbience(glm::vec3 ambience);
    void addDiffuse(glm::vec3 diffuse);
    void addSpecular(glm::vec3 specular);
    void addRoughness(float roughness);
    void addAlpha(float alpha);

private:
    std::string m_name{};

    ref<renderer::Texture2D> m_ambientMap   = nullptr;
    ref<renderer::Texture2D> m_diffuseMap   = nullptr;
    ref<renderer::Texture2D> m_specularMap  = nullptr;
    ref<renderer::Texture2D> m_roughnessMap = nullptr;
    ref<renderer::Texture2D> m_alphaMap     = nullptr;
    std::optional<glm::vec3> m_ambience     = std::nullopt;
    std::optional<glm::vec3> m_diffuse      = std::nullopt;
    std::optional<glm::vec3> m_specular     = std::nullopt;
    std::optional<float> m_roughness        = std::nullopt;
    std::optional<float> m_alpha            = std::nullopt;
};

} // namespace core

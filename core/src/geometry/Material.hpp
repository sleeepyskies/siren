#pragma once

#include "renderer/Texture.hpp"
#include "utilities/spch.hpp"

namespace siren::geometry
{

struct Material {
    Material()  = default;
    ~Material() = default;

    Ref<renderer::Texture2D> baseColorMap = nullptr;
    glm::vec4 baseColorFactor{ 1 };

    Ref<renderer::Texture2D> metallicRoughnessMap = nullptr;
    float metallicFactor{ 0 };  // assume materials are by default non metallic
    float roughnessFactor{ 1 }; // assume surfaces are by default fully diffuse aka very rough

    Ref<renderer::Texture2D> emissionMap = nullptr;
    glm::vec3 emissionColor{ 0 };

    Ref<renderer::Texture2D> occlusionMap = nullptr;
    float occlusionStrength{ 1 };

    Ref<renderer::Texture2D> normalMap = nullptr;
    float normalScale{ 1 };
};

} // namespace siren::geometry

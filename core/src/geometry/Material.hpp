#pragma once

#include "renderer/Texture.hpp"
#include "utilities/spch.hpp"

namespace core::geometry
{

struct Material final : public assets::Asset {
    explicit Material(const std::string& name) : Asset(name)
    {
    }
    Material()           = delete;
    ~Material() override = default;

    ASSET_TYPE(assets::AssetType::Material);

    Ref<renderer::Texture2D> baseColorMap = nullptr;
    glm::vec4 baseColorFactor{ 1 };

    Ref<renderer::Texture2D> metallicRoughnessMap = nullptr;
    float metallicFactor{ 1 };
    float roughnessFactor{ 1 };

    Ref<renderer::Texture2D> emissionMap = nullptr;
    glm::vec3 emissionColor{ 0 };

    Ref<renderer::Texture2D> occlusionMap = nullptr;
    float occlusionStrength{ 1 };

    Ref<renderer::Texture2D> normalsMap = nullptr;
    float normalScale{ 1 };
};

} // namespace core::geometry

module;

#include <string>

export module siren.asset:texture_asset;

import :asset;

import siren.render;

namespace siren::asset {

/**
 * @brief An asset holding an @ref Image and an @ref ImageSampler.
 */
export struct TextureAsset : Asset {
    /** @brief The name of the Texture. */
    std::string name;
    /** @brief The underlying Image of the Texture. */
    render::Image image;
    /** @brief The underlying ImageSampler of the Texture. */
    render::Sampler sampler;

    /** @brief Constructs a new Texture. */
    TextureAsset(
        const std::string& name,
        render::Image&& image,
        render::Sampler&& sampler
    ) : name(name), image(std::move(image)), sampler(std::move(sampler)) { }
};

} // namespace siren::asset

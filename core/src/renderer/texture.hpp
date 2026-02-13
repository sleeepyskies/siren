#pragma once

#include "utilities/spch.hpp"
#include "assets/asset.hpp"
#include "resources/image.hpp"
#include "resources/sampler.hpp"


namespace siren::core
{
/**
 * @brief An asset holding an Image and an ImageSampler.
 */
struct Texture : Asset {
    std::string name; ///< @brief The name of the Texture.
    Image image;      ///< @brief The underlying Image of the Texture.
    Sampler sampler;  ///< @brief The underlying ImageSampler of the Texture.

    Texture(
        const std::string& name,
        Image&& image,
        Sampler&& sampler
    ) : name(name), image(std::move(image)), sampler(std::move(sampler)) { }
};
} // namespace siren::core

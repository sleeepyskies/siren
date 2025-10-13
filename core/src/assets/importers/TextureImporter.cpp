#include "TextureImporter.hpp"

#include <stb_image.h>

namespace siren::assets::TextureImporter
{

Ref<renderer::Texture2D> importTexture2D(const Path& path)
{
    if (!exists(path)) {
        wrn("File does not exist at {}", path.string());
        return nullptr;
    }

    stbi_set_flip_vertically_on_load(true);
    int w, h, c;
    const stbi_uc* data = stbi_load(absolute(path).string().data(), &w, &h, &c, STBI_default);
    const std::vector<Byte> buf{ data, data + w * h * c };

    if (!data) {
        wrn("Could not load image at {}", path.string());
        return nullptr;
    }

    // just take over default values
    renderer::Image2DSampler sampler{};
    Ref<renderer::Texture2D> texture = makeRef<renderer::Texture2D>(buf, sampler, w, h);

    return texture;
}

} // namespace siren::assets::TextureImporter

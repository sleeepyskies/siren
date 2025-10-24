#include "Material.hpp"

namespace siren::core
{

MaterialKey Material::generateMaterialKey() const
{
    // TODO: this should have more as the shaders become more complex, but for now we only have 1
    // variant, maybe could also store ShadingMode in the Material
    return MaterialKey{};
}

bool Material::hasTexture(const TextureType type) const
{
    return textureArray[static_cast<size_t>(type)] != nullptr;
}

Ref<Texture2D> Material::getTexture(const TextureType type) const
{
    if (!hasTexture(type)) {
        return nullptr;
    }
    return textureArray[static_cast<size_t>(type)];
}

void Material::pushTexture(const Ref<Texture2D>& texture, TextureType type)
{
    textureArray[static_cast<size_t>(type)] = texture;
}

} // namespace siren::core
#include "Material.hpp"

namespace siren::core
{

MaterialKey Material::getMaterialKey() const
{
    NotImplemented;
}

void Material::invalidateMaterialKey() const
{
    m_materialKey = Nothing;
}

bool Material::hasTexture(TextureType type) const
{
    return m_textureArray[static_cast<size_t>(type)] != AssetHandle::invalid();
}

void Material::setTexture(TextureType type, const AssetHandle textureHandle)
{
    m_textureArray[static_cast<size_t>(type)] = textureHandle;
}

Maybe<AssetHandle> Material::getTexture(TextureType type, AssetHandle textureHandle) const
{
    if (hasTexture(type)) {
        return m_textureArray[static_cast<size_t>(type)];
    }
    return Nothing;
}

} // namespace siren::core
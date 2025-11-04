#include "Material.hpp"


namespace siren::core
{

MaterialKey Material::getMaterialKey() const
{
    if (m_materialKey) {
        return *m_materialKey;
    }

    m_materialKey = MaterialKey{ };

    return *m_materialKey;
}

void Material::invalidateMaterialKey() const
{
    m_materialKey = Nothing;
}

bool Material::hasTexture(TextureRole type) const
{
    return m_textureArray[static_cast<size_t>(type)] != AssetHandle::invalid();
}

void Material::setTexture(TextureRole type, const AssetHandle textureHandle)
{
    m_textureArray[static_cast<size_t>(type)] = textureHandle;
}

Maybe<AssetHandle> Material::getTexture(TextureRole type) const
{
    if (hasTexture(type)) {
        return m_textureArray[static_cast<size_t>(type)];
    }
    return Nothing;
}

} // namespace siren::core

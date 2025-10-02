#include "Asset.hpp"

#include <core/Application.hpp>
#include <random>

namespace core::assets
{

AssetHandle::AssetHandle()
{
    static std::mt19937_64 random(std::chrono::system_clock::now().time_since_epoch().count());
    m_uuid = random();
}

bool AssetHandle::operator==(const AssetHandle& other) const
{
    return this->m_uuid == other.m_uuid;
}

} // namespace core::assets
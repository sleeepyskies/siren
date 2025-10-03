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

AssetHandle::AssetHandle(const uint64_t uuid) : m_uuid(uuid)
{
}

AssetHandle AssetHandle::invalid()
{
    return AssetHandle{ 0 };
}

bool AssetHandle::operator==(const AssetHandle& other) const
{
    return this->m_uuid == other.m_uuid;
}

bool AssetHandle::operator<(const AssetHandle& other) const
{
    return this->m_uuid < other.m_uuid;
}

} // namespace core::assets

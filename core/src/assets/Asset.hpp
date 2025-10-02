#pragma once

namespace core::assets
{

/**
 * @brief A universally unique way to identify an asset in siren.
 */
class AssetHandle
{
public:
    /// @brief Assigns a random uuid on construction
    AssetHandle();
    bool operator==(const AssetHandle&) const;

private:
    /// @brief We use just a 64-bit handle as this is far more than enough for our use case.
    /// Around 607 million assets would need to be imported for a 0.1% chance of a duplicate uuid
    uint64_t m_uuid = 0;
};

/**
 * @brief A base class for that all asset types should inherit from. Makes the asset usable by the
 * @ref AssetManager.
 */
class Asset
{
public:
    explicit Asset(const std::string& name) : m_name(name)
    {
    }
    virtual ~Asset() = 0;

private:
    std::string m_name{};
};

} // namespace core::assets

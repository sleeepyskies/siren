module;

#include <string>

export module siren.asset:asset_config;

namespace siren::asset {

/**
 * @struct AssetConfig
 * @brief Configuration of the asset module.
*/
export struct AssetConfig {
    /// @brief Root directory for asset loading
    std::string path = "./assets";
};

} // namespace siren::asset

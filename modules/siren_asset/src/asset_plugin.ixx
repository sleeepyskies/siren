export module siren.asset.asset_plugin;

import siren.asset.asset_server;
import siren.asset.asset_config;
import siren.app.plugin;
import siren.app.app;

namespace siren::asset {

/**
 * @class Plugin
 * @brief Interface for defining custom extensions to the siren engine.
 * Plugins may modify the @ref siren::App by adding resources and systems.
 */
export class AssetPlugin final : Plugin {
public:
    explicit AssetPlugin(AssetConfig& config);

    auto construct(App& app) const -> void;
    auto shutdown(App& app) const -> void;

private:
    AssetConfig m_config;
};

auto AssetPlugin::construct(App& app) const -> void {
    app.add_resource<AssetServer>(m_config);
}

auto AssetPlugin::shutdown(App& app) const -> void {
    app.add_resource<AssetServer>(m_config);
}
} // namespace siren::asset

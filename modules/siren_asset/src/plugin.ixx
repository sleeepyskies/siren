export module siren.asset:plugin;

import :asset_server;
import :asset_config;

import siren.app;

namespace siren::asset {

/**
 * @class AssetPlugin
 * @brief Interface for defining custom extensions to the siren engine.
 * Plugins may modify the @ref siren::App by adding resources and systems.
 */
export class AssetPlugin final : public Plugin {
public:
    explicit AssetPlugin(const AssetConfig& config = { }) : m_config(config) { }

    auto construct(App& app) const -> void override;
    auto shutdown(App& app) const -> void override;

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

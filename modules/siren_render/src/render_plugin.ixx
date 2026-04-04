export module siren.render.render_plugin;

import siren.app;
import siren.render.device;
import siren.render.renderer;

namespace siren::render {

export enum class Backend {
    OpenGL,
    Auto,
};

export struct RenderConfig {
    Backend backend;
};

export class RenderPlugin final : public Plugin {
public:
    explicit RenderPlugin(const RenderConfig& config);

    auto construct(App& app) const -> void override;
    auto shutdown(App& app) const -> void override;

private:
    RenderConfig m_config;
};

RenderPlugin::RenderPlugin(const RenderConfig& config) : m_config(config) { }

auto RenderPlugin::construct(App& app) const -> void {
    app.add_resource<Renderer>().add_resource<Device>();
}

auto RenderPlugin::shutdown(App& app) const -> void {
    app.remove_resources<Renderer, Device>();
}
} // namespace siren::render

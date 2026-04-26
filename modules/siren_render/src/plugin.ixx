export module siren.render:plugin;

import siren.app;
import siren.window;

import :device;
import :render_thread;
import :renderer;

namespace siren::render {

export enum class Backend {
    OpenGL,
    Auto,
};

export struct RenderConfig {
    Backend backend = Backend::Auto;
};

export class RenderPlugin final : public Plugin {
public:
    explicit RenderPlugin(const RenderConfig& config = { });

    auto construct(App& app) const -> void override;
    auto shutdown(App& app) const -> void override;

private:
    RenderConfig m_config;
};

RenderPlugin::RenderPlugin(const RenderConfig& config) : m_config(config) { }

auto RenderPlugin::construct(App& app) const -> void {
    RenderThread::init(app.resource<window::Window>()->glfw_handle());
    app.add_resource<Renderer>();
}

auto RenderPlugin::shutdown(App& app) const -> void {
    app.remove_resources<Renderer, Device>();
    RenderThread::get().wait_until_idle();
}
} // namespace siren::render

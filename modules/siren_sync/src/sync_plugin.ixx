export module siren.sync.sync_plugin;

import siren.sync;
import siren.app.app;
import siren.app.plugin;

namespace siren::sync {

class SyncPlugin final : public Plugin {
    SyncPlugin() = default;

    auto construct(App& app) const -> void;
    auto shutdown(App& app) const -> void;
};

auto SyncPlugin::construct(App& app) const -> void {
    app.world().add_resource<ThreadPool>()->init();
}

auto SyncPlugin::shutdown(App& app) const -> void {
    app.remove_resources<ThreadPool>();
}

} // namespace siren::sync {

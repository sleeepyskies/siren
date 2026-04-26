#include <entt/entt.hpp>

import siren.app;
import siren.window;
import siren.ecs;
import siren.render.plugin;
import siren.input.plugin;
import siren.asset.plugin;
import siren.asset.asset_server;
import siren.time;

import siren.render.mesh;

using namespace siren;

int main(const int argc, char* argv[]) {
    App{ }
           .add_plugin<log::LogPlugin>()
           .add_plugin(window::WindowPlugin{ })
           .add_plugin(input::InputPlugin{ })
           .add_plugin(asset::AssetPlugin{ })
           .add_plugin(render::RenderPlugin{ })
           .add_plugin(time::TimePlugin{ })
           .run();

    return 0;
}

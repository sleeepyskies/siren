import siren.app.app;
import siren.window;
import siren.ecs;
import siren.input;
import siren.asset;
import siren.time;

import siren.render.mesh;

using namespace siren;

auto init_system(
    const ecs::Query<const render::Mesh&, const Transform&> query,
    ecs::Resource<time::DeltaTime&> delta,
    ecs::Resource<asset::AssetServer&> asset_server
) -> void {
    //
}

int main(const int argc, char* argv[]) {
    App{ }
           .add_plugin(window::WindowPlugin{ })
           .add_plugin(input::InputPlugin{ })
           .add_plugin(renderer::RenderPlugin{ })
           .add_plugin(assets::AssetPlugin{ })
           .add_plugin(time::TimePlugin{ })
           .add_system(init_system)
           .run();

    return 0;
}

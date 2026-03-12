import siren.app.app;

auto init_system(
    const Query<const Mesh&, const Transform&> query,
    Res<DeltaTime> delta,
    Res<AssetServer> asset_server
) -> void {
    //
}

int main(const int argc, char* argv[]) {
    siren::App{ }
           .add_plugin(window::WindowPlugin{ })
           .add_plugin(input::InputPlugin{ })
           .add_plugin(renderer::RenderPlugin{ })
           .add_plugin(assets::AssetPlugin{ })
           .add_plugin(time::TimePlugin{ })
           .add_system(init_system)
           .run();

    return 0;
}

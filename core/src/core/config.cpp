#include "config.hpp"

#include "node.hpp"
#include "file_system.hpp"


namespace siren
{

void from_node(const fkyaml::node& node, Config& cfg) {
    // assets
    if (node.contains("assets")) {
        auto& assets = node["assets"];
        if (assets.contains("path")) cfg.assets_config.path = assets["path"].get_value<std::string>();
    }

    // window
    if (node.contains("window")) {
        auto& win = node["window"];
        if (win.contains("title")) cfg.window_config.title = win["title"].get_value<std::string>();
        if (win.contains("width")) cfg.window_config.width = win["width"].get_value<u32>();
        if (win.contains("height")) cfg.window_config.height = win["height"].get_value<u32>();
        if (win.contains("fullscreen")) cfg.window_config.fullscreen = win["fullscreen"].get_value<bool>();
        if (win.contains("vsync")) cfg.window_config.vsync = win["vsync"].get_value<bool>();
        if (win.contains("decorated")) cfg.window_config.decorated = win["decorated"].get_value<bool>();
        if (win.contains("resizable")) cfg.window_config.resizable = win["resizable"].get_value<bool>();
        if (win.contains("transparent")) cfg.window_config.transparent = win["transparent"].get_value<bool>();
    }

    // renderer
    if (node.contains("renderer")) {
        auto& ren = node["renderer"];
        if (ren.contains("backend")) cfg.renderer_config.backend = ren["backend"].get_value<std::string>();
    }

    // logging
    if (node.contains("logging")) {
        auto& log = node["logging"];
        if (log.contains("level")) cfg.logging_config.level = log["level"].get_value<std::string>();
        if (log.contains("console")) cfg.logging_config.console = log["console"].get_value<bool>();
        if (log.contains("file-path")) cfg.logging_config.file_path = log["file-path"].get_value<std::string>();
    }
}

auto Config::create(const int argc, char* argv[]) -> Config {
    std::string config_path;

    if (argc == 1) {
        config_path = "./config.siren";
    } else if (argc == 3) {
        if (std::strcmp("--config", argv[1]) != 0) {
            std::println(stderr, "detected unknown argument: {}", argv[1]);
            std::exit(1);
        }
        config_path = argv[2];
    } else {
        std::println(stderr, "usage: siren [--config path/to/config.siren]");
        std::exit(1);
    }

    std::ifstream ifs{ std::filesystem::absolute(config_path) };

    auto root = fkyaml::node::deserialize(ifs);
    return root.get_value<Config>();
}

} // namespace siren

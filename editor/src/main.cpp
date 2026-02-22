#include "EditorApp.hpp"
#include "core/app.hpp"
#include "core/config.hpp"

int main(const int argc, char* argv[]) {
    const auto config = siren::Config::create(argc, argv);
    siren::core::App::create<siren::editor::EditorApp>(config)->run();
    return 0;
}

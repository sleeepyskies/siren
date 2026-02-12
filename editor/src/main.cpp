#include "EditorApp.hpp"
#include "core/app.hpp"
#include "utilities/UUID.hpp"
#include <slog.hpp>

int main(const int argc, char* argv[]) {
    slog::log_level = slog::Level::Trace;
    siren::utilities::UUID::setSeed(69420); // should be build mode dependent

    siren::core::App::create<siren::editor::EditorApp>(
                {
                    .name = "Siren Editor",
                    .renderAPI = siren::core::App::Description::RenderAPI::OpenGL,
                    .OS = siren::core::App::Description::OS::Windows
                }
            )
           .run();

    return 0;
}

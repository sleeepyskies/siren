#include "SandboxApp.hpp"
#include "core/App.hpp"
#include "utilities/UUID.hpp"
#include "slog.hpp"

int main(const int argc, char* argv[])
{
    slog::logLevel = slog::Level::Trace;
    siren::utilities::UUID::setSeed(69420); // should be build mode dependent

    siren::core::App::create<siren::sandbox::SandboxApp>(
                {
                    .name = "Sandbox",
                    .renderAPI = siren::core::App::Properties::RenderAPI::OpenGL,
                    .OS = siren::core::App::Properties::OS::Windows
                }
            )
            .run();

    return 0;
}

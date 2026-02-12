#include "SandboxApp.hpp"
#include "core/app.hpp"
#include "utilities/UUID.hpp"

int main(const int argc, char* argv[]) {
    siren::utilities::UUID::setSeed(69420); // should be build mode dependent

    siren::core::App::create<siren::sandbox::SandboxApp>(
                {
                    .name = "Sandbox",
                    .renderAPI = siren::core::App::Description::RenderAPI::OpenGL,
                    .OS = siren::core::App::Description::OS::Windows
                }
            )
           .run();

    return 0;
}

#include "SandboxApp.hpp"
#include "core/app.hpp"
#include "utilities/Uuid.hpp"

int main(const int argc, char* argv[]) {
    siren::utilities::Uuid::set_seed(69420); // should be build mode dependent

    siren::core::App::create<siren::sandbox::SandboxApp>(
                {
                    .name = "Sandbox",
                    .renderAPI = siren::core::App::AppDescriptor::RenderAPI::OpenGL,
                    .OS = siren::core::App::AppDescriptor::OS::Windows
                }
            )
           .run();

    return 0;
}

#include "ui.hpp"

#include "utilities/ImGui.hpp"
#include "window/WindowModule.hpp"


namespace siren::ui
{

void initUI(const core::WindowModule& window)
{
    // todo: enabling multi view port does not work and is buggy in siren atm

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // configuration flags / load fonts / setup style here
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.FontGlobalScale = 1.2f;                            // increase font size

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window.handle()), true);
    ImGui_ImplOpenGL3_Init();
}

void shutdownUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void beginUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void endUI()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace siren::ui

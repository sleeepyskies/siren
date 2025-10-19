#include "UiManager.hpp"

#include "ImGui.hpp"

#include "core/Application.hpp"

namespace siren::ui
{

void UiManager::init(const bool multiViewport)
{
    // todo: enabling multi view port does not work and is buggy in siren atm
    m_multiViewportEnabled = multiViewport;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // configuration flags / load fonts / setup style here
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.FontGlobalScale = 1.2f;                            // increase font size
    if (multiViewport) { io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; }

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(core::Application::get().getWindow().handle(), true);
    ImGui_ImplOpenGL3_Init();

    m_setup = true;
}

void UiManager::shutDown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_setup = false;
}

void UiManager::begin() const
{
    SirenAssert(m_setup, "Cannot begin UIManager when not setup!");

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UiManager::end() const
{
    SirenAssert(m_setup, "Cannot end UIManager when not setup!");

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (!m_multiViewportEnabled) { return; }
    // only needed to call if multi viewport enabled
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

} // namespace siren::ui

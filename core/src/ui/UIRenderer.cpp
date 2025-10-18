#include "UIRenderer.hpp"

#include "ImGui.hpp"

#include "core/Application.hpp"

namespace siren::ui
{

void UIRenderer::init(const bool multiViewport)
{
    m_multiViewportEnabled = multiViewport;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // configuration flags / load fonts / setup style here
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(core::Application::get().getWindow().handle(), true);
    ImGui_ImplOpenGL3_Init();

    m_setup = true;
}

void UIRenderer::shutDown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_setup = false;
}

void UIRenderer::begin() const
{
    SirenAssert(m_setup, "Cannot begin UIManager when not setup!");

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIRenderer::end() const
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

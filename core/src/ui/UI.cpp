#include "UI.hpp"

#include <imgui_internal.h>

#include "filesystem/FileSystemModule.hpp"

#include "utilities/ImGui.hpp"
#include "window/WindowModule.hpp"
#include "fonts/fonts.hpp"


namespace siren::UI
{

void initialize()
{
    const auto& window = core::window();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window.handle()), true);
    ImGui_ImplOpenGL3_Init();

    // io flags
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // docking setup
    {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
        io.ConfigFlags |= ImGuiDockNodeFlags_NoCloseButton; // hides the close button on the right
        io.ConfigFlags |= ImGuiDockNodeFlags_NoTabBar;      // hides the arrow on the tab bar
    }
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewports ///< BUGGY

    detail::loadFonts();
    detail::setupStyle();
}

void shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void end()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // ImGui::UpdatePlatformWindows();
    // ImGui::RenderPlatformWindowsDefault();
}

void detail::loadFonts()
{
    const ImGuiIO& io = ImGui::GetIO();
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;

    // regular fonts
    {
        font::Regular = io.Fonts->AddFontFromMemoryTTF(
            const_cast<u8*>(interRegular),
            interRegularSize,
            16.f,
            &fontConfig
        );
        font::Bold = io.Fonts->AddFontFromMemoryTTF(
            const_cast<u8*>(interBold),
            interBoldSize,
            16.f,
            &fontConfig
        );
    }

    // icons
    {
        icon::Far = io.Fonts->AddFontFromMemoryTTF(
            const_cast<u8*>(fontAwesomeRegular),
            fontAwesomeRegularSize,
            16.f,
            &fontConfig
        );
        icon::Fas = io.Fonts->AddFontFromMemoryTTF(
            const_cast<u8*>(fontAwesomeSolid),
            fontAwesomeSolidSize,
            16.f,
            &fontConfig
        );
    }
}

void detail::setupStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();

    style.ItemSpacing      = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(5, 5);
    style.IndentSpacing    = 20;
    style.GrabMinSize      = 10;

    style.WindowRounding = 2;
    style.ChildRounding  = 2;
    style.FrameRounding  = 2;
    style.PopupRounding  = 2;
}

} // namespace siren::ui

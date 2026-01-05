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

    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window.GetHandle()), true);
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

    // main
    {
        style.WindowPadding     = ImVec2(8, 8);
        style.FramePadding      = ImVec2(5, 3);
        style.ItemSpacing       = ImVec2(5, 8);
        style.ItemInnerSpacing  = ImVec2(5, 5);
        style.TouchExtraPadding = ImVec2(0, 0);
        style.IndentSpacing     = 25;
        style.GrabMinSize       = 10;
    }

    // borders
    {
        style.WindowBorderSize = 0;
        style.ChildBorderSize  = 0;
        style.PopupBorderSize  = 0;
        style.FrameBorderSize  = 0;
    }

    // rounding
    {
        style.WindowRounding = 2;
        style.ChildRounding  = 2;
        style.FrameRounding  = 2;
        style.PopupRounding  = 2;
        style.GrabRounding   = 2;
    }

    // scrollbar
    {
        style.ScrollbarSize     = 13;
        style.ScrollbarRounding = 8;
        style.ScrollbarPadding  = 2;
    }

    // tabs
    {
        style.TabBorderSize                    = 0;
        style.TabBarBorderSize                 = 2;
        style.TabBarOverlineSize               = 1;
        style.TabMinWidthBase                  = 1;
        style.TabMinWidthShrink                = 80;
        style.TabCloseButtonMinWidthSelected   = -1;
        style.TabCloseButtonMinWidthUnselected = 0;
        style.TabRounding                      = 0;
    }

    // tables
    { }

    // trees
    {
        style.TreeLinesFlags = ImGuiTreeNodeFlags_DrawLinesFull;
        style.TreeLinesSize  = 2;
    }

    // windows
    {
        style.WindowTitleAlign         = ImVec2(0.00f, 0.50f);
        style.WindowBorderHoverPadding = 10;
        style.WindowMenuButtonPosition = ImGuiDir_None;
    }

    // widgets
    {
        style.ColorButtonPosition     = ImGuiDir_Right;
        style.SelectableTextAlign     = ImVec2(0.02f, 0.00f);
        style.SeparatorTextBorderSize = 3;
        style.SeparatorTextAlign      = ImVec2(0.50f, 0.10f);
        style.SeparatorTextPadding    = ImVec2(20, 3);
        style.ImageBorderSize         = 0;
    }

    // docking
    {
        style.DockingSeparatorSize = 2;
    }

    // tooltips
    {
        style.HoverFlagsForTooltipMouse |= ImGuiHoveredFlags_DelayNone;
        style.HoverFlagsForTooltipNav |= ImGuiHoveredFlags_DelayNone;
    }

    // misc
    {
        style.DisplaySafeAreaPadding = ImVec2(0, 0);
    }

    // color theme (incomplete), should load into separate function/variable
    {
        ImVec4* colors                        = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
        colors[ImGuiCol_PopupBg]              = ImVec4(0.04f, 0.04f, 0.06f, 1.00f);
        colors[ImGuiCol_Border]               = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_FrameBg]              = ImVec4(0.16f, 0.22f, 0.30f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.54f, 0.87f, 1.00f, 0.39f);
        colors[ImGuiCol_FrameBgActive]        = ImVec4(0.54f, 0.87f, 1.00f, 0.67f);
        colors[ImGuiCol_TitleBg]              = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
        colors[ImGuiCol_TitleBgActive]        = ImVec4(0.04f, 0.04f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.04f, 0.04f, 0.06f, 0.71f);
        colors[ImGuiCol_MenuBarBg]            = ImVec4(0.06f, 0.07f, 0.10f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.52f, 1.00f, 1.00f, 0.56f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.52f, 1.00f, 1.00f, 0.76f);
        colors[ImGuiCol_CheckMark]            = ImVec4(1.00f, 0.80f, 0.42f, 1.00f);
        colors[ImGuiCol_SliderGrab]           = ImVec4(0.51f, 0.67f, 1.00f, 1.00f);
        colors[ImGuiCol_Button]               = ImVec4(0.51f, 0.67f, 1.00f, 0.52f);
        colors[ImGuiCol_ButtonHovered]        = ImVec4(0.51f, 0.67f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive]         = ImVec4(0.51f, 0.67f, 1.00f, 0.68f);

    }
}

} // namespace siren::ui

#include "ui.hpp"

#include "filesystem/FileSystemModule.hpp"

#include "utilities/ImGui.hpp"
#include "window/WindowModule.hpp"


namespace siren::ui
{

void initUI(const core::WindowModule& window)
{
    const auto& fs = core::filesystem();
    // todo: enabling multi view port does not work and is buggy in siren atm

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // configuration flags / load fonts / setup style here
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    // font settings
    {
        const Path fontPath = fs.getEngineRoot() / "assets" / "fonts" / "inter" / "static" / "Inter_24pt-Regular.ttf";
        io.FontGlobalScale  = 0.75f; // increase font size
        ImFont* font        = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 24);
        ImGui::PushFont(font);
    }

    // global styling
    {
        ImGuiStyle& style = ImGui::GetStyle();

        style.ItemSpacing      = ImVec2(8, 4);
        style.ItemInnerSpacing = ImVec2(5, 5);
        style.IndentSpacing    = 20;
        style.GrabMinSize      = 10;

        style.WindowRounding = 2;
        style.ChildRounding  = 2;
        style.FrameRounding  = 2;
        style.PopupRounding  = 2;

    }

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

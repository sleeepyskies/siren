#include "DebugLayer.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "event/AppEvent.hpp"
#include "event/KeyEvent.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

namespace siren
{

DebugLayer::DebugLayer()
{
}

DebugLayer::~DebugLayer()
{
}

void DebugLayer::onAttach()
{
}

void DebugLayer::onDetach()
{
}

void DebugLayer::onUpdate(const float delta)
{
    const float now = core::Application::get().getTime().elapsed();
    m_accumulatedTime += (now - m_previousFrameElapsed);
    m_previousFrameElapsed = now;

    if (m_accumulatedTime < 0.5) { return; }

    // update window with FPS only every 0.1s
    core::Window& window  = core::Application::get().getWindow();
    const float fps       = 1 / delta;
    const float frametime = delta * 1000;
    window.setTitle(std::format("siren FPS: {}, Frame time: {}ms", fps, frametime));
    m_accumulatedTime = 0;
}

void DebugLayer::onRender()
{
}

void DebugLayer::onUiRender()
{
    if (!m_active) { return; }

    // TL;DR; this demo is more complicated than what most users you would normally use.
    // If we remove all options we are showcasing, this demo would become a simple call to
    // ImGui::DockSpaceOverViewport() !! In this specific demo, we are not using
    // DockSpaceOverViewport() because:

    static bool opt_fullscreen                = true;
    static bool opt_padding                   = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking,
    // otherwise any change of dockspace/settings would lead to windows being stuck in limbo and
    // never being visible.
    if (!opt_padding) { ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); }
    ImGui::Begin("DockSpace Demo", &m_active, window_flags);
    if (!opt_padding) { ImGui::PopStyleVar(); }

    if (opt_fullscreen) { ImGui::PopStyleVar(2); }

    // Submit the DockSpace
    // REMINDER: THIS IS A DEMO FOR ADVANCED USAGE OF DockSpace()!
    // MOST REGULAR APPLICATIONS WILL SIMPLY WANT TO CALL DockSpaceOverViewport(). READ COMMENTS
    // ABOVE.
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    // Show demo options and help
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            // Disabling fullscreen would allow the window to be moved to the front of other
            // windows, which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) !=
                                    0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
            }
            if (ImGui::MenuItem("Flag: NoDockingSplit",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit;
            }
            if (ImGui::MenuItem("Flag: NoUndocking",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking;
            }
            if (ImGui::MenuItem(
                    "Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode",
                                "",
                                (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0,
                                opt_fullscreen)) {
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Close", NULL, false, m_active != NULL)) { m_active = false; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::TextUnformatted(
                "This demo has nothing to do with enabling docking!"
                "\n"
                "This demo only demonstrate the use of ImGui::DockSpace() which allows you to "
                "manually\ncreate a docking node _within_ another window."
                "\n"
                "Most application can simply call ImGui::DockSpaceOverViewport() and be done with "
                "it.");
            ImGui::Separator();
            ImGui::TextUnformatted(
                "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!"
                "\n"
                "- Drag from window title bar or their tab to dock/undock."
                "\n"
                "- Drag from window menu button (upper-left button) to undock an entire node (all "
                "windows)."
                "\n"
                "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)"
                "\n"
                "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)");
            ImGui::Separator();
            ImGui::TextUnformatted("More details:");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextLinkOpenURL("Docking Wiki page",
                                   "https://github.com/ocornut/imgui/wiki/Docking");
            ImGui::BulletText("Read comments in ShowExampleAppDockSpace()");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void DebugLayer::onEvent(event::Event& e)
{
    event::EventHandler keyPressEventHandler(e);
    keyPressEventHandler.handle<event::KeyPressEvent>([this](const event::KeyPressEvent& e) {
        // enable mouse when pressing esc
        if (e.getKeycode() == core::KeyCode::ESC) {
            // toggle pause TestLayer
            if (m_active) {
                event::LayerResumeEvent resumeEvent{ "TestLayer" };
                m_eventCallback(resumeEvent);
                m_active = false;
            } else {
                event::LayerPauseEvent pauseEvent{ "TestLayer" };
                m_eventCallback(pauseEvent);
                m_active = true;
            }
            return true;
        }

        return false;
    });
}

std::string DebugLayer::getName() const
{
    return "DebugLayer";
}

} // namespace siren

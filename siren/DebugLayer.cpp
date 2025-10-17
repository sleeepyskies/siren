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
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // configuration flags / load fonts / setup style here
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    ImGui_ImplGlfw_InitForOpenGL(core::Application::get().getWindow().handle(), true);
    ImGui_ImplOpenGL3_Init();
}

void DebugLayer::onDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
    // pass
}

void DebugLayer::onUiRender()
{
    if (!m_active) { return; }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

#include "MainMenuBar.hpp"

#include "utilities/ImGui.hpp"


namespace siren
{

void editor::MainMenuBar::onUiRender()
{

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu(); //
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) { }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) { } // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) { }
            if (ImGui::MenuItem("Copy", "CTRL+C")) { }
            if (ImGui::MenuItem("Paste", "CTRL+V")) { }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

std::string editor::MainMenuBar::getName() const
{
    return m_name;
}

} // namespace siren

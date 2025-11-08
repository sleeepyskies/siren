#pragma once

#include "utilities/ImGui.hpp"
#include "utilities/spch.hpp"
#include "EditorState.hpp"


namespace siren::editor
{

/**
 * @brief A DockPanel in the Siren Editor is a UI component belonging to the dock space.
 * Furthermore, all Panels must be docked, and cannot free float.
 */
class Panel
{
public:
    explicit Panel(EditorState* state) : m_state(state) { }

    virtual ~Panel() { }

    /// @brief Handles wrapping each call to Panel::draw in ImGui::Begin and ImGui::End for simplicity.
    void onRender()
    {
        ImGui::Begin(getName().c_str());
        draw();
        ImGui::End();
    }

    /// @brief Called once a frame.
    virtual void onUpdate(float delta) { }

    /// @brief Called once a frame. This is where panels should implement their UI drawing logic, not in onRender().
    virtual void draw() = 0;

    virtual std::string getName() const = 0;

protected:
    EditorState* m_state = nullptr;
};

} // namespace siren::editor

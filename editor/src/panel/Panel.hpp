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
        // afaik no way to enforce minimum dock window size,
        // maybe check hazel engine to see how they do it
        // https://github.com/ocornut/imgui/issues/6295
        ImGui::SetNextWindowSizeConstraints(getMinSize(), getMaxSize());
        ImGui::Begin(getName().c_str());
        draw();
        ImGui::End();
    }

    /// @brief Called once a frame.
    virtual void onUpdate(float delta) { }

    /// @brief Called once a frame. This is where panels should implement their UI drawing logic, not in onRender().
    virtual void draw() = 0;

    /// @brief The name of the panel. Will be used when rendering.
    virtual std::string getName() const = 0;

    /// @brief The minimum size the panel may have.
    /// @note Currently not enforced.
    virtual ImVec2 getMinSize() const { return ImVec2(-1, -1); }

    /// @brief The maximum size the panel may have.
    /// @note Currently not enforced.
    virtual ImVec2 getMaxSize() const { return ImVec2(-1, -1); }

protected:
    EditorState* m_state = nullptr;

};

} // namespace siren::editor

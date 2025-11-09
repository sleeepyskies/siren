#pragma once

#include "Panel.hpp"
#include "ecs/core/Scene.hpp"


namespace siren::editor
{

class SceneHierarchyPanel final : public Panel
{
public:
    explicit SceneHierarchyPanel(EditorState* state) : Panel(state) { }

    ~SceneHierarchyPanel() override = default;

    void draw() override;

    std::string getName() const override { return "Scene Hierarchy"; }

    ImVec2 getMinSize() const override { return ImVec2(1500, -1); }

private:
    // UI Functions

    void drawToolbar();
    void drawPanel();
    void drawEntity(core::EntityHandle entity);

    // Utility Functions

    bool shouldDeselect() const;
    core::EntityHandle createEntity();
    core::EntityHandle addChild(core::EntityHandle parent);
    void deleteEntity(core::EntityHandle entity);

    // Local State
    bool m_renaming                          = false; ///< If an entity is currently being renamed.
    bool m_exitRename                        = false; ///< Indicates if we should stop renaming selected entity.
    std::string m_renameBuffer               = "";    ///< String buffer for entity name
    const ImGuiTreeNodeFlags m_baseNodeFlags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanFullWidth |
            ImGuiTreeNodeFlags_AllowOverlap |
            ImGuiTreeNodeFlags_FramePadding; ///< Base TreeNode flags.
};

} // namespace siren::editor

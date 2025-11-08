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

private:
    // UI Functions

    void drawToolbar();
    void drawPanel();
    void drawEntity(core::EntityHandle entity);

    // Utility Functions

    core::EntityHandle createEntity();
    core::EntityHandle addChild(core::EntityHandle parent);
    void deleteEntity(core::EntityHandle entity);

    // Local State
    bool m_renaming = false; ///< If an entity is currently being renamed.
};

} // namespace siren::editor

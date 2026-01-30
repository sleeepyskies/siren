#pragma once
#include "Panel.hpp"
#include "ecs/core/World.hpp"


namespace siren::editor
{


class InspectorPanel final : public Panel
{
public:
    explicit InspectorPanel(EditorState* state) : Panel(state) { }

    ~InspectorPanel() override = default;

    void draw() override;

    std::string getName() const override { return "Inspector Panel"; }

private:
    void drawComponents() const;
    void drawAddComponents() const;
    void addComponentToEntity() const;

    void drawMeshComponent() const;
    void drawTransformComponent() const;

    // local state

    mutable int m_currentCoreComponent                    = 0;
    static constexpr const char* const m_coreComponents[] = {
        "Mesh",
        "Script",
        "Transform",
        "Camera",
        "Directional Light",
        "Point Light",
        "Spot Light",
        "Skybox Light"
    };

    mutable int m_currentMesh                     = 0;
    static constexpr const char* const m_meshes[] = {
        "Empty",
        "Custom Mesh",
        "Capsule Mesh",
        "Cube Mesh",
        "Plane Mesh"
    };
};

} // namespace siren::editor

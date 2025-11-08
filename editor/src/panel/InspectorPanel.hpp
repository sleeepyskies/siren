#pragma once
#include "Panel.hpp"
#include "ecs/core/Scene.hpp"


namespace siren::editor
{


class InspectorPanel final : public Panel
{
public:
    explicit InspectorPanel(EditorState* state) : Panel(state) { }

    ~InspectorPanel() override = default;

    void draw() override;

    std::string getName() const override { return "Inspector Panel"; }
};

} // namespace siren::editor

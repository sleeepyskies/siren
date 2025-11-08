#pragma once
#include "Panel.hpp"


namespace siren::editor
{

class MaterialEditorPanel final : public Panel
{
public:
    explicit MaterialEditorPanel(EditorState* state) : Panel(state) { }

    ~MaterialEditorPanel() override = default;

    void draw() override;

    std::string getName() const override { return "Material Editor"; }
};

} // siren::editor

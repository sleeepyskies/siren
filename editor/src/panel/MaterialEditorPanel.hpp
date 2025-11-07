#pragma once
#include "Panel.hpp"


namespace siren::core
{
class Scene;
}


namespace siren::editor
{

class MaterialEditorPanel final : public Panel
{
public:
    explicit MaterialEditorPanel(const Ref<core::Scene>& scene) : m_scene(scene) { }

    void draw() override;

    std::string getName() const override { return "Material Editor"; }

private:
    Ref<core::Scene> m_scene = nullptr;

};

} // siren::editor

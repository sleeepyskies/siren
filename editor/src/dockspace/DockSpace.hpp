#pragma once

#include "panel/DockPanel.hpp"
#include "utilities/spch.hpp"

namespace siren::ecs
{
class Scene;
}

namespace siren::editor
{

class DockSpace
{
public:
    explicit DockSpace(const Ref<ecs::Scene>& scene);

    void onUpdate(float delta);
    void onRender() const;
    void onUiRender() const;

private:
    Ref<ecs::Scene> m_scene = nullptr;
    // todo: how do we remove panels?
    std::vector<Uref<DockPanel>> m_panels{};
};

} // namespace siren::editor

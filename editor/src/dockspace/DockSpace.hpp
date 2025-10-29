#pragma once

#include "panel/DockPanel.hpp"
#include "utilities/spch.hpp"


// clang-format off
namespace siren::core{ class Scene; }
// clang-format on

namespace siren::editor
{

class DockSpace
{
public:
    explicit DockSpace(const Ref<core::Scene>& scene);

    void onUpdate(float delta);
    void onRender() const;
    void onUiRender() const;

private:
    Ref<core::Scene> m_scene = nullptr;
    // todo: how do we remove panels?
    std::vector<Own<DockPanel>> m_panels{ };
};

} // namespace siren::editor

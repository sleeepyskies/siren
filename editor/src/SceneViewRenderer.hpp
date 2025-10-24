#pragma once

#include "geometry/primitives/PlaneMesh.hpp"
#include "utilities/spch.hpp"

// todo: go through files removing unneeded includes, make fwd_XXX.hpp files instead

// clang-format off
namespace siren::ecs { class Scene; }
namespace siren::renderer { class FrameBuffer; }
// clang-format on

namespace siren::editor
{
class EditorCamera;

class SceneViewRenderer
{
public:
    SceneViewRenderer();

    void render(const Ref<ecs::Scene>& scene, const Ref<EditorCamera>& camera,
                const Ref<renderer::FrameBuffer>& frameBuffer) const;

private:
    bool m_renderGridLines            = true;
    Ref<geometry::PlaneMesh> m_plane = nullptr;
};

} // namespace siren::editor

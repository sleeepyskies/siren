#pragma once

#include "utilities/spch.hpp"
#include "geometry/Mesh.hpp"

// todo: go through files removing unneeded includes, make fwd_XXX.hpp files instead

// clang-format off
namespace siren::core{ class Scene; class FrameBuffer; }
// clang-format on

namespace siren::editor
{
class EditorCamera;

class SceneViewRenderer
{
public:
    SceneViewRenderer();

    void render(
        const Ref<core::Scene>& scene,
        const Ref<EditorCamera>& camera,
        const Ref<core::FrameBuffer>& frameBuffer
        ) const;

private:
    bool m_renderGridLines  = true;
    Ref<core::Mesh> m_plane = nullptr;
};

} // namespace siren::editor

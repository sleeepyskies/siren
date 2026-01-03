#pragma once

#include "utilities/spch.hpp"
#include "geometry/Mesh.hpp"
#include "geometry/Primitive.hpp"

#include "renderer/material/Material.hpp"
#include "renderer/shaders/Shader.hpp"

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
        const core::Scene& scene,
        const Ref<EditorCamera>& camera,
        const Ref<core::FrameBuffer>& frameBuffer
    ) const;

private:
    struct EditorGrid
    {
        bool enabled                 = true;
        Ref<core::Material> material = nullptr;
        Ref<core::VertexArray> mesh  = nullptr;
        Ref<core::Shader> shader     = nullptr;
        glm::mat4 transform{ 1 };
    } m_editorGrid;

    void createEditorGrid();


};

} // namespace siren::editor

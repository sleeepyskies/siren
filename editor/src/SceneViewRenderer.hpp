#pragma once

#include "utilities/spch.hpp"
#include "geometry/mesh.hpp"
#include "geometry/primitive.hpp"

#include "../../core/src/renderer/pbr_material.hpp"
#include "renderer/shaders/shader.hpp"

// todo: go through files removing unneeded includes, make fwd_XXX.hpp files instead

// clang-format off
namespace siren::core{ class World; class Framebuffer; }
// clang-format on

namespace siren::editor
{
class EditorCamera;

class SceneViewRenderer
{
public:
    SceneViewRenderer();

    void render(
        const core::World& scene,
        const Ref<EditorCamera>& camera,
        const Ref<core::Framebuffer>& frameBuffer
    ) const;

private:
    struct EditorGrid
    {
        bool enabled                 = true;
        Ref<core::PBRMaterial> material = nullptr;
        Ref<core::VertexArray> mesh  = nullptr;
        Ref<core::Shader> shader     = nullptr;
        glm::mat4 transform{ 1 };
    } m_editorGrid;

    void createEditorGrid();


};

} // namespace siren::editor

#include "SceneHierarchyPanel.hpp"

#include "EditorContextComponent.hpp"
#include "ui/UI.hpp"
#include "ecs/Components.hpp"
#include "ui/fonts/FontAwesome.hpp"
#include "utilities/ImGui.hpp"


namespace siren::editor
{

// ============================================================================
// == MARK: Helper functions
// ============================================================================

static void addChild(core::Scene* scene, const core::EntityHandle parent, const core::EntityHandle child)
{
    auto& childHierarchy = scene->emplace<core::HierarchyComponent>(child);
    SirenAssert(!childHierarchy.parent, "Cannot overwrite a child's parent");
    childHierarchy.parent = parent;

    if (parent) {
        auto& parentHierarchy = scene->emplace<core::HierarchyComponent>(parent);
        parentHierarchy.children.push_back(child);
    }
}

static void deleteEntityNode(core::Scene* scene, const core::EntityHandle entity)
{
    const auto hierarchy = scene->getSafe<core::HierarchyComponent>(entity);

    if (!hierarchy) { return; }

    for (const auto& child : hierarchy->children) {
        deleteEntityNode(scene, child);
    }
    scene->destroy(entity);
}

static std::string getEntityName(core::Scene* scene, const core::EntityHandle entity)
{
    const auto tag = scene->getSafe<core::TagComponent>(entity);
    return tag ? tag->tag : "Unnamed";
}

static void renderEntityNode(core::Scene* scene, const core::EntityHandle entity, EditorContextComponent* editorContext)
{
    // if we are currently renaming this entity
    static bool rename = false;
    // if we have just started renaming this frame
    const bool startRename = ImGui::IsKeyDown(ImGuiKey_F2) || ImGui::IsItemClicked();
    const bool selected    = editorContext && editorContext->selectedEntity == entity;

    std::string name      = getEntityName(scene, entity);
    const auto& hierarchy = scene->get<core::HierarchyComponent>(entity); // must have this component

    ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanFullWidth |
            ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (hierarchy.children.empty()) { flags |= ImGuiTreeNodeFlags_Leaf; }
    if (selected) { flags |= ImGuiTreeNodeFlags_Selected; }

    const bool isOpen = ImGui::TreeNodeEx(
        reinterpret_cast<void*>(static_cast<intptr_t>(entity.id())),
        flags,
        "%s",
        name.c_str()
    );

    if (selected && (rename || startRename)) {
        rename = true;
        ImGui::InputText("", &name);
    }

    if (ImGui::IsItemClicked()) {
        if (editorContext) { editorContext->selectedEntity = entity; }
    }

    if (isOpen) {
        for (const auto& child : hierarchy.children) {
            renderEntityNode(scene, child, editorContext);
        }
        ImGui::TreePop();
    }
}


void SceneHierarchyPanel::draw()
{
    drawToolbar();
    drawPanel();
}

void SceneHierarchyPanel::drawToolbar()
{
    const auto editorContext = m_scene->getSingletonSafe<EditorContextComponent>();
    if (!editorContext) { return; }

    ImGui::PushFont(UI::icon::Fas);

    if (ImGui::Button(FAS_PLUS)) {
        const core::EntityHandle parent = editorContext->selectedEntity;
        const auto entity               = m_scene->create();
        m_scene->emplace<core::TagComponent>(entity, "Unnamed");
        m_scene->emplace<core::TransformComponent>(entity);
        addChild(m_scene.get(), parent, entity);
    }

    ImGui::SameLine();

    if (ImGui::Button(FAS_MINUS)) {
        if (editorContext->selectedEntity) {
            deleteEntityNode(m_scene.get(), editorContext->selectedEntity);
            editorContext->selectedEntity = core::EntityHandle::invalid();
        }
    }

    ImGui::PopFont();
}

void SceneHierarchyPanel::drawPanel()
{
    const auto editorContext = m_scene->getSingletonSafe<EditorContextComponent>();
    if (!editorContext) { return; }

    // fixme: this sorting is kinda dumb lol "10" < "2"
    auto entities = m_scene->getWith<core::HierarchyComponent>();
    std::sort(
        entities.begin(),
        entities.end(),
        [this] (const core::EntityHandle e1, const core::EntityHandle e2) {
            const auto tag1 = m_scene->getSafe<core::TagComponent>(e1);
            const auto tag2 = m_scene->getSafe<core::TagComponent>(e2);
            return (tag1 ? tag1->tag : "") < (tag2 ? tag2->tag : "");
        }
    );

    for (const auto& e : entities) {
        const auto hierarchy = m_scene->getSafe<core::HierarchyComponent>(e);
        if (!hierarchy || hierarchy->parent) { continue; }

        renderEntityNode(m_scene.get(), e, editorContext);

        // deselect on click off
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            editorContext->selectedEntity = core::EntityHandle::invalid();
        }
    }
}

} // namespace siren::editor

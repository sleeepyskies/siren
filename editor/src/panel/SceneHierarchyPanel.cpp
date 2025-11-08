#include "SceneHierarchyPanel.hpp"

#include "ui/UI.hpp"
#include "ecs/Components.hpp"
#include "ui/fonts/FontAwesome.hpp"
#include "utilities/ImGui.hpp"


namespace siren::editor
{

// ============================================================================
// == MARK: Helper functions
// ============================================================================
static std::string getEntityName(const core::Scene& scene, const core::EntityHandle entity)
{
    const auto tag = scene.getSafe<core::TagComponent>(entity);
    return tag ? tag->tag : "Unnamed";
}

// ============================================================================
// == MARK: Member functions
// ============================================================================


void SceneHierarchyPanel::draw()
{
    drawToolbar();
    drawPanel();
}

void SceneHierarchyPanel::drawToolbar()
{
    core::EntityHandle& selectedEntity = m_state->selectedEntity;

    ImGuiSiren::ScopedFont fas(UI::icon::Fas);

    if (ImGui::Button(FAS_PLUS)) { addChild(createEntity()); }

    ImGui::SameLine();

    if (ImGui::Button(FAS_MINUS)) {
        if (!selectedEntity) { return; }
        deleteEntity(selectedEntity);
        selectedEntity = core::EntityHandle::invalid();
    }
}

void SceneHierarchyPanel::drawPanel()
{
    auto& scene = m_state->scene;

    // fixme: this sorting is kinda dumb lol "10" < "2"
    auto entities = scene.getWith<core::HierarchyComponent>();
    std::sort(
        entities.begin(),
        entities.end(),
        [&scene] (const core::EntityHandle e1, const core::EntityHandle e2) {
            const auto tag1 = scene.getSafe<core::TagComponent>(e1);
            const auto tag2 = scene.getSafe<core::TagComponent>(e2);
            return (tag1 ? tag1->tag : "") < (tag2 ? tag2->tag : "");
        }
    );

    for (const auto& entity : entities) {
        const auto hierarchy = scene.getSafe<core::HierarchyComponent>(entity);
        if (!hierarchy || hierarchy->parent) { continue; }

        drawEntity(entity);

        // deselect on click off
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            m_state->selectedEntity = core::EntityHandle::invalid();
        }
    }
}

void SceneHierarchyPanel::drawEntity(const core::EntityHandle entity)
{
    const auto& scene    = m_state->scene;
    auto& selectedEntity = m_state->selectedEntity;

    // if we have just started renaming this frame
    const bool startRename = ImGui::IsKeyDown(ImGuiKey_F2) || ImGui::IsItemClicked();

    std::string name      = getEntityName(scene, entity);
    const auto& hierarchy = scene.get<core::HierarchyComponent>(entity); // must have this component

    ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanFullWidth |
            ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (hierarchy.children.empty()) { flags |= ImGuiTreeNodeFlags_Leaf; }
    if (selectedEntity) { flags |= ImGuiTreeNodeFlags_Selected; }

    const bool isOpen = ImGui::TreeNodeEx(
        reinterpret_cast<void*>(static_cast<intptr_t>(entity.id())),
        flags,
        "%s",
        name.c_str()
    );

    if (selectedEntity && (m_renaming || startRename)) {
        m_renaming = true;
        ImGui::InputText("", &name);
    }

    if (ImGui::IsItemClicked()) { selectedEntity = entity; }

    if (isOpen) {
        for (const auto& child : hierarchy.children) {
            drawEntity(child);
        }
        ImGui::TreePop();
    }
}

core::EntityHandle SceneHierarchyPanel::createEntity()
{
    auto& scene                     = m_state->scene;
    const core::EntityHandle entity = scene.create();
    scene.emplace<core::TagComponent>(entity, "Unnamed");
    scene.emplace<core::HierarchyComponent>(entity);
    scene.emplace<core::TransformComponent>(entity);
    return entity;
}

core::EntityHandle SceneHierarchyPanel::addChild(const core::EntityHandle parent)
{
    if (!parent) { return core::EntityHandle::invalid(); }

    auto& scene                    = m_state->scene;
    const core::EntityHandle child = createEntity();

    auto& parentHierarchy = scene.get<core::HierarchyComponent>(parent);
    auto& childHierarchy  = scene.get<core::HierarchyComponent>(child);

    parentHierarchy.children.push_back(child);
    childHierarchy.parent = parent;

    return child;
}

void SceneHierarchyPanel::deleteEntity(const core::EntityHandle entity)
{
    auto& scene           = m_state->scene;
    const auto* hierarchy = scene.getSafe<core::HierarchyComponent>(entity);
    if (!hierarchy) { return; }
    for (const auto& child : hierarchy->children) {
        deleteEntity(child);
    }
    scene.destroy(entity);
}

} // namespace siren::editor

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

static void setEntityName(const core::Scene& scene, const core::EntityHandle entity, const std::string& name)
{
    const auto tag = scene.getSafe<core::TagComponent>(entity);
    if (!tag) return;
    tag->tag = name;
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

    if (ImGui::Button(FAS_PLUS)) {
        if (selectedEntity) {
            addChild(selectedEntity);
        } else {
            createEntity();
        }
    }

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
    ImGuiSiren::ScopedStyleVarY yPad(ImGuiStyleVar_FramePadding, 4);

    const auto& scene             = m_state->scene;
    const bool thisEntitySelected = m_state->selectedEntity == entity;
    // if we are renaming this entity
    const bool renamingThisEntity = thisEntitySelected && m_renaming;

    m_renameBuffer        = getEntityName(scene, entity);
    const auto& hierarchy = scene.get<core::HierarchyComponent>(entity); // must have this component

    ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanFullWidth |
            ImGuiTreeNodeFlags_OpenOnDoubleClick |
            ImGuiTreeNodeFlags_FramePadding;
    if (hierarchy.children.empty()) { flags |= ImGuiTreeNodeFlags_Leaf; }
    if (thisEntitySelected) { flags |= ImGuiTreeNodeFlags_Selected; }

    const bool nodeOpen = ImGui::TreeNodeEx(reinterpret_cast<void*>(entity.id()), flags, "");

    // first click -> select entity
    // second click on selected entity -> start rename entity
    if (ImGui::IsItemClicked()) {
        if (thisEntitySelected) {
            m_renaming = true;
        } else {
            m_state->selectedEntity = entity;
            m_renaming              = false;
        }
    }

    // todo: determine which icon to draw here. maybe a separate function call for this?
    ImGuiSiren::InlineIcon(UI::icon::Fas, FAS_VIDEO);

    // F2 rename hotkey
    if (ImGui::IsKeyDown(ImGuiKey_F2) && thisEntitySelected) { m_renaming = true; }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    if (renamingThisEntity) {
        ImGuiSiren::ScopedStyleColor bgblack(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 255));
        m_renaming = true;
        ImGui::InputText("##RenameEntity", &m_renameBuffer, ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemDeactivated()) {
            setEntityName(scene, entity, m_renameBuffer);
            m_renaming = false;
        }
    } else {
        ImGui::TextUnformatted(m_renameBuffer.c_str());
    }

    if (nodeOpen) {
        for (const auto& child : hierarchy.children) { drawEntity(child); }
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

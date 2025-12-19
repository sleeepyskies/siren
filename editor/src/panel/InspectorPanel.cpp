#include "InspectorPanel.hpp"

#include "GuiMeta.hpp"
#include "MetaRegistry.hpp"

#include "ecs/Components.hpp"
#include "ecs/core/Scene.hpp"
#include "utilities/spch.hpp"


namespace siren::editor
{
void InspectorPanel::draw()
{
    const core::EntityHandle selectedEntity = m_state->selectedEntity;

    if (!selectedEntity) {
        ImGuiSiren::Text("No entity selected.");
        return;
    }

    const auto& tag = m_state->scene.get<core::TagComponent>(selectedEntity); // selected entity must have tag
    ImGuiSiren::Text("{}", tag.tag);

    drawComponents();
    drawAddComponents();
}

void InspectorPanel::drawComponents() const
{
    auto isType = []<typename T> (const entt::meta_type& type) -> bool {
        return type.info().hash() == entt::type_id<T>().hash();
    };

    auto& scene               = m_state->scene;
    const auto selectedEntity = m_state->selectedEntity;

    MetaRegistry mr = MetaRegistry::get();
    for (const auto meta : mr.getComponents()) {
        if (!mr.hasComponent(meta.t, scene, selectedEntity)) { continue; }

        ImGui::SeparatorText(meta.t.name());

        // components with special controller rendering
        {
            if (meta.t.id() == entt::resolve<core::MeshComponent>().id()) {
                continue;
            }
        }

        for (const auto& [id, field] : meta.t.data()) {
            const entt::meta_type fieldType = field.type();
            entt::meta_any fieldValue       = field.get(mr.getComponent(meta.t, scene, selectedEntity));
            const GuiMeta& guiMeta          = field.custom();

            if (!fieldValue) { continue; }

            // todo: all basic data types should be setup here, sofar only float, vec3 (special case for colors) and string is done

            if (isType.operator()<float>(fieldType)) {
                float* valPtr = fieldValue.try_cast<float>();
                if (!valPtr) continue;

                if (guiMeta.isSlider()) {
                    const auto& [min, max] = guiMeta.getSliderParams();
                    ImGui::SliderFloat(field.name(), valPtr, min, max);
                } else if (guiMeta.isInput()) {
                    const auto& [step, stepFast] = guiMeta.getInputParams();
                    ImGui::InputFloat(field.name(), valPtr, step, stepFast);
                } else if (guiMeta.isDrag()) {
                    const auto& [speed, min, max] = guiMeta.getDragParams();
                    ImGui::DragFloat(field.name(), valPtr, speed, min, max);
                }

                field.set(mr.getComponent(meta.t, scene, selectedEntity), *valPtr);
            }

            if (isType.operator()<std::string>(fieldType)) {
                std::string* valPtr = fieldValue.try_cast<std::string>();
                if (!valPtr) { continue; }

                ImGui::InputText(field.name(), valPtr, ImGuiInputTextFlags_AutoSelectAll);
                if (ImGui::IsItemDeactivated() || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                    field.set(mr.getComponent(meta.t, scene, selectedEntity), *valPtr);
                }
            }

            if (isType.operator()<glm::vec3>(fieldType)) {
                glm::vec3* valPtr = fieldValue.try_cast<glm::vec3>();
                if (!valPtr) continue;

                if (guiMeta.isColor()) {
                    ImGui::ColorPicker3(field.name(), &valPtr->x);
                    field.set(mr.getComponent(meta.t, scene, selectedEntity), *valPtr);
                    continue;
                }

                if (guiMeta.isSlider()) {
                    const auto& [min, max] = guiMeta.getSliderParams();
                    ImGui::SliderFloat3(field.name(), &valPtr->x, min, max);
                } else if (guiMeta.isInput()) {
                    // todo: ImGuiSiren::InputFloat3 for step support?????
                    ImGui::InputFloat3(field.name(), &valPtr->x);
                } else if (guiMeta.isDrag()) {
                    const auto& [speed, min, max] = guiMeta.getDragParams();
                    ImGui::DragFloat3(field.name(), &valPtr->x, speed, min, max);
                }

                field.set(mr.getComponent(meta.t, scene, selectedEntity), *valPtr);
            }
        }
    }
}

void InspectorPanel::drawAddComponents() const
{
    if (ImGui::Button("Add Component +")) {
        ImGui::OpenPopup("AddComponentModal");
    }

    if (ImGui::BeginPopupModal("AddComponentModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGuiSiren::Text("Core Components");
        ImGui::ListBox(
            "##Core Components",
            &m_currentCoreComponent,
            m_coreComponents,
            IM_ARRAYSIZE(m_coreComponents),
            10
        );

        ImGuiSiren::Text("User Defined Components");

        if (ImGui::Button("Add")) {
            ImGui::CloseCurrentPopup();
            addComponentToEntity();
        }

        ImGui::EndPopup();
    }
}

void InspectorPanel::addComponentToEntity() const
{
    const char* componentString = m_coreComponents[m_currentCoreComponent];

    // todo: better solution that a bunch of if checks here lol
    if (std::strcmp("Mesh", componentString) == 0) {
        m_state->scene.emplace<core::MeshComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Script", componentString) == 0) {
        m_state->scene.emplace<core::ScriptContainerComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Transform", componentString) == 0) {
        m_state->scene.emplace<core::TransformComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Camera", componentString) == 0) {
        Todo;
    }
    if (std::strcmp("Directional Light", componentString) == 0) {
        m_state->scene.emplace<core::DirectionalLightComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Spot Light", componentString) == 0) {
        m_state->scene.emplace<core::SpotLightComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Point Light", componentString) == 0) {
        m_state->scene.emplace<core::PointLightComponent>(m_state->selectedEntity);
    }
    if (std::strcmp("Skybox Light", componentString) == 0) {
        m_state->scene.emplace<core::SkyLightComponent>(m_state->selectedEntity);
    }
}
} // namespace siren::editor


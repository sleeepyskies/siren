#pragma once

#include "../../../../core/src/utilities/ImGui.hpp"
#include "utilities/spch.hpp"

namespace siren::editor
{

/**
 * @brief A DockPanel in the Siren Editor is a UI component belonging to the dock space.
 * Furthermore, all Panels must be docked, and cannot free float.
 */
class DockPanel
{
public:
    virtual ~DockPanel() = default;
    virtual void renderUi()
    {
        ImGui::Begin(getName().c_str());
        onUiRender();
        ImGui::End();
    }
    virtual void onUpdate(float delta){};
    virtual void onRender() const {};
    virtual void onUiRender()           = 0;
    virtual std::string getName() const = 0;

protected:
};

} // namespace siren::editor

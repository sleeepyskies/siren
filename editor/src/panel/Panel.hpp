#pragma once

#include "utilities/ImGui.hpp"
#include "utilities/spch.hpp"


namespace siren::editor
{

/**
 * @brief A DockPanel in the Siren Editor is a UI component belonging to the dock space.
 * Furthermore, all Panels must be docked, and cannot free float.
 */
class Panel
{
public:
    virtual ~Panel() = default;

    virtual void renderUi()
    {
        ImGui::Begin(getName().c_str());
        onRender();
        ImGui::End();
    }

    virtual void onUpdate(float delta) { }

    virtual void onRender() { }

    virtual std::string getName() const = 0;
};

} // namespace siren::editor

#pragma once

#include "ui/ImGui.hpp"
#include "utilities/spch.hpp"

namespace siren::editor
{

/**
 * @brief A StaticUiElement in the Siren Editor is a UI component that is fixed and is always
 * rendered.
 */
class StaticUiElement
{
public:
    virtual ~StaticUiElement()          = default;
    virtual void renderUi() const       = 0;
    virtual std::string getName() const = 0;
};

} // namespace siren::editor
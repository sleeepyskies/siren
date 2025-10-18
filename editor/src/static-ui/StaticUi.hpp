#pragma once

#include "element/StaticUiElement.hpp"
#include "utilities/spch.hpp"

namespace siren::editor
{

class StaticUi
{
public:
    StaticUi();

    void onUiRender() const;

private:
    std::vector<Uref<StaticUiElement>> m_elements{};
};

} // namespace siren::editor

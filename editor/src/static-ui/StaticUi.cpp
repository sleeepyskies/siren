#include "StaticUi.hpp"

#include "element/MainMenuBar.hpp"

namespace siren::editor
{

StaticUi::StaticUi()
{
    // setup static ui here
    m_elements.emplace_back(std::make_unique<MainMenuBar>());
}

void StaticUi::onUiRender() const
{
    for (const auto& element : m_elements) { element->renderUi(); }
}

} // namespace siren::editor

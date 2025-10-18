#pragma once

#include "StaticUiElement.hpp"

namespace siren::editor
{

class MainMenuBar final : public StaticUiElement
{
public:
    void renderUi() const override;
    std::string getName() const override;

private:
    std::string m_name = "Main Menu Bar";
};

} // namespace siren::editor

#pragma once

#include "Widget.hpp"

namespace siren::editor
{

class MainMenuBar final : public Widget
{
public:
    void onUiRender() override;
    std::string getName() const override;

private:
    std::string m_name = "Main Menu Bar";
};

} // namespace siren::editor

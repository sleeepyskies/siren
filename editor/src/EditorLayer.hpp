#pragma once
#include <core/Layer.hpp>

namespace siren::editor
{

class EditorLayer final : public core::Layer
{
public:
    void onAttach() override;
    void onDetach() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onUiRender() override;
    void onEvent(event::Event& e) override;
    std::string getName() const override;

private:
};

} // namespace siren::editor

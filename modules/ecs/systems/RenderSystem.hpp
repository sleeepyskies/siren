#pragma once

#include "ecs/core/System.hpp"


namespace siren::core
{

class RenderSystem final : public System {
public:
    void on_render(Scene& scene) override;
};

} // namespace siren::ecs

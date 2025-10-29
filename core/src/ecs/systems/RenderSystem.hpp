#pragma once

#include "ecs/core/System.hpp"

namespace siren::core
{

class RenderSystem final : public System
{
public:
    void onRender(Scene& scene) override;
};

} // namespace siren::ecs

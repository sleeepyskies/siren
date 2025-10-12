#pragma once

#include "assets/Asset.hpp"
#include "ecs/core/System.hpp"

namespace siren::ecs
{

class RenderSystem final : public System
{
public:
    void onRender(Scene& scene) override;
};

} // namespace siren::ecs

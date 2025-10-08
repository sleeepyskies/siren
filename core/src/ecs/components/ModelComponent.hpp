#pragma once

#include "assets/Asset.hpp"
#include "ecs/core/Component.hpp"

namespace siren::ecs
{
struct ModelComponent final : Component {
    assets::AssetHandle modelHandle = assets::AssetHandle::invalid();
};

} // namespace siren::ecs

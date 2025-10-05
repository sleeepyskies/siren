#pragma once

#include "assets/Asset.hpp"
#include "ecs/Component.hpp"

namespace siren::ecs
{
struct ModelComponent final : secs::Component {
    assets::AssetHandle modelHandle = assets::AssetHandle::invalid();
};

} // namespace siren::ecs

#pragma once

#include "assets/Asset.hpp"
#include "ecs/core/Component.hpp"

namespace siren::core
{
struct MeshComponent final : Component
{
    AssetHandle meshHandle = AssetHandle::invalid();

    explicit MeshComponent(const AssetHandle handle) : meshHandle(handle)
    {
    }
};

} // namespace siren::ecs

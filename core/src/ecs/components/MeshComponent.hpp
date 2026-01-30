#pragma once

#include "assets/Asset.hpp"
#include "ecs/core/Component.hpp"


namespace siren::core
{
struct MeshComponent final : Component
{
    StrongHandle meshHandle = StrongHandle::invalid();

    explicit MeshComponent(const StrongHandle handle) : meshHandle(handle) { }

    MeshComponent() { }
};

} // namespace siren::ecs

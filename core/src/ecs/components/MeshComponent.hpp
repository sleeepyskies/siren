#pragma once

#include "assets/Asset.hpp"
#include "ecs/core/Component.hpp"
#include "geometry/Mesh.hpp"


namespace siren::core
{
struct MeshComponent final : Component {
    StrongHandle<Mesh> meshHandle = StrongHandle<Mesh>::invalid();
    explicit MeshComponent(const StrongHandle<Mesh>& handle) : meshHandle(handle) { }
    MeshComponent() { }
};

} // namespace siren::ecs

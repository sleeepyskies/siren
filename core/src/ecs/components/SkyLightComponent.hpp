#pragma once

#include "ecs/core/Component.hpp"
#include "renderer/buffer/VertexArray.hpp"
#include "geometry/primitive.hpp"


namespace siren::core
{
struct SkyLightComponent final : Component
{
    AssetHandle cubeMapHandle = AssetHandle::invalid();

    explicit SkyLightComponent(const AssetHandle handle) : cubeMapHandle(handle) { }

    SkyLightComponent() : cubeMapHandle(AssetHandle::invalid()) { }
};
}

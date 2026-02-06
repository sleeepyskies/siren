#pragma once

#include "ecs/core/Component.hpp"


namespace siren::core
{
struct Texture;

struct SkyLightComponent final : Component {
    StrongHandle<Texture> cubeMapHandle = StrongHandle::invalid();

    explicit SkyLightComponent(const StrongHandle handle) : cubeMapHandle(handle) { }

    SkyLightComponent() : cubeMapHandle(StrongHandle::invalid()) { }
};
}

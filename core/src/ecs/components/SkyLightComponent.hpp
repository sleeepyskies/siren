#pragma once

#include "ecs/core/Component.hpp"
#include "renderer/resources/Texture.hpp"


namespace siren::core
{
struct SkyLightComponent final : Component {
    StrongHandle<Texture> cubeMapHandle = StrongHandle<Texture>::invalid();
    explicit SkyLightComponent(const StrongHandle<Texture>& handle) : cubeMapHandle(handle) { }
    SkyLightComponent() { }
};
}

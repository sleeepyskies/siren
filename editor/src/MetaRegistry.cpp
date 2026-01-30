#include "MetaRegistry.hpp"

#include "GuiMeta.hpp"

#include "ecs/components/DirectionalLightComponent.hpp"
#include "ecs/components/MeshComponent.hpp"
#include "ecs/components/PointLightComponent.hpp"
#include "ecs/components/SkyLightComponent.hpp"
#include "ecs/components/SpotLightComponent.hpp"
#include "ecs/components/TagComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/EntityHandle.hpp"


namespace siren::editor
{
MetaRegistry::MetaRegistry()
{
    registerPrimitives();
    registerTypes();
    registerComponents();
}

void MetaRegistry::registerPrimitives()
{
    reflect<int>("int");
    reflect<float>("float");
    reflect<double>("double");

    reflect<char>("char");
    reflect<std::string>("std::string");

    reflect<byte>("byte");
    reflect<u8>("u8");
    reflect<u16>("u16");
    reflect<u32>("u32");
    reflect<u64>("u64");
    reflect<i8>("i8");
    reflect<i16>("i16");
    reflect<i32>("i32");
    reflect<i64>("i64");

    reflect<glm::vec2>("vec2");
    reflect<glm::vec3>("vec3");
    reflect<glm::vec4>("vec4");

    reflect<glm::mat2>("mat2");
    reflect<glm::mat3>("mat3");
    reflect<glm::mat4>("mat4");
}

void MetaRegistry::registerTypes()
{
    reflect<core::StrongHandle>("AssetHandle");
    reflect<core::EntityHandle>("EntityHandle");
    reflect<Vector<core::EntityHandle>>("Vector<EntityHandle>");
}

void MetaRegistry::registerComponents()
{
    // todo: camera components

    reflect<core::DirectionalLightComponent>("DirectionalLightComponent")
            .data<&core::DirectionalLightComponent::color>("color")
            .custom<GuiMeta>(GuiMeta::color())
            .data<&core::DirectionalLightComponent::direction>("direction")
            .custom<GuiMeta>(GuiMeta::drag());

    reflect<core::MeshComponent>("MeshComponent")
            .data<&core::MeshComponent::meshHandle>("meshHandle")
            .custom<GuiMeta>(GuiMeta::none());

    reflect<core::PointLightComponent>("PointLightComponent")
            .data<&core::PointLightComponent::color>("color")
            .custom<GuiMeta>(GuiMeta::color())
            .data<&core::PointLightComponent::position>("position")
            .custom<GuiMeta>(GuiMeta::drag());

    reflect<core::SkyLightComponent>("SkyLightComponent")
            .data<&core::SkyLightComponent::cubeMapHandle>("cubeMapHandle")
            .custom<GuiMeta>(GuiMeta::none());

    reflect<core::SpotLightComponent>("SpotLightComponent")
            .data<&core::SpotLightComponent::innerCone>("innerCone")
            .custom<GuiMeta>(GuiMeta::drag(0.1f, 0.f, 50.f))
            .data<&core::SpotLightComponent::outerCone>("outerCone")
            .custom<GuiMeta>(GuiMeta::drag(0.1f, 0.f, 50.f))
            .data<&core::SpotLightComponent::color>("color")
            .custom<GuiMeta>(GuiMeta::color())
            .data<&core::SpotLightComponent::position>("position")
            .custom<GuiMeta>(GuiMeta::drag());

    reflect<core::TransformComponent>("TransformComponent")
            .data<&core::TransformComponent::rotation>("rotation")
            .custom<GuiMeta>(GuiMeta::drag(0.02, 0, glm::pi<float>()))
            .data<&core::TransformComponent::scale>("scale")
            .custom<GuiMeta>(GuiMeta::drag())
            .data<&core::TransformComponent::translation>("translation")
            .custom<GuiMeta>(GuiMeta::drag());
}
} // siren::editor

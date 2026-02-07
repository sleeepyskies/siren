#include "ThirdPersonCamera.hpp"

#include "../../../core/src/input/InputModule.hpp"
#include "ecs/components/ThirdPersonCameraComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/World.hpp"


namespace siren::editor
{

void ThirdPersonCamera::onReady()
{
    core::input().setMouseMode(core::MouseMode::Locked);
}

void ThirdPersonCamera::onShutdown()
{
    core::input().setMouseMode(core::MouseMode::Visible);
}

void ThirdPersonCamera::onUpdate(const float delta)
{
    auto& transform = get<core::TransformComponent>();         // the models transform
    auto& camera    = get<core::ThirdPersonCameraComponent>(); // the camera

    const glm::vec2 mouseDelta = core::input().getDeltaMousePosition();
    const float deltaSens      = camera.sensitivity * camera.rotationSpeed;

    camera.yaw -= mouseDelta.x * deltaSens;
    camera.pitch += mouseDelta.y * deltaSens;
    camera.pitch =
            glm::clamp(camera.pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

    const glm::vec3 focalPoint = transform.translation + camera.focalOffset;

    glm::vec3 offset;
    offset.x = camera.distanceOffset * std::cos(camera.pitch) * std::sin(camera.yaw);
    offset.y = camera.distanceOffset * std::sin(camera.pitch);
    offset.z = camera.distanceOffset * std::cos(camera.pitch) * std::cos(camera.yaw);

    camera.position      = focalPoint + offset;
    camera.viewDirection = glm::normalize(focalPoint - camera.position);

    const float modelYaw = std::atan2(
        camera.position.x - transform.translation.x,
        camera.position.z - transform.translation.z
    );
    transform.rotation.y = modelYaw;
}

void ThirdPersonCamera::onPause()
{
    core::input().setMouseMode(core::MouseMode::Visible);
}

void ThirdPersonCamera::onResume()
{
    core::input().setMouseMode(core::MouseMode::Locked);
}

} // namespace siren

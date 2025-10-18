#include "ThirdPersonCamera.hpp"

#include "core/Input.hpp"
#include "ecs/components/ThirdPersonCameraComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/Scene.hpp"

namespace siren
{

void ThirdPersonCamera::onReady()
{
    core::Input::setMouseMode(core::MouseMode::LOCKED);
}

void ThirdPersonCamera::onShutdown()
{
    core::Input::setMouseMode(core::MouseMode::VISIBLE);
}

void ThirdPersonCamera::onUpdate(const float delta)
{
    auto& transform = get<ecs::TransformComponent>();         // the models transform
    auto& camera    = get<ecs::ThirdPersonCameraComponent>(); // the camera

    const glm::vec2 mouseDelta = core::Input::getDeltaMousePosition();
    const float deltaSens      = camera.sensitivity * camera.rotationSpeed;

    camera.yaw -= mouseDelta.x * deltaSens;
    camera.pitch += mouseDelta.y * deltaSens;
    camera.pitch =
        glm::clamp(camera.pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

    const glm::vec3 focalPoint = transform.position + camera.focalOffset;

    glm::vec3 offset;
    offset.x = camera.distanceOffset * std::cos(camera.pitch) * std::sin(camera.yaw);
    offset.y = camera.distanceOffset * std::sin(camera.pitch);
    offset.z = camera.distanceOffset * std::cos(camera.pitch) * std::cos(camera.yaw);

    camera.position      = focalPoint + offset;
    camera.viewDirection = glm::normalize(focalPoint - camera.position);

    const float modelYaw = std::atan2(camera.position.x - transform.position.x,
                                      camera.position.z - transform.position.z);
    transform.rotation   = glm::quat(glm::vec3(0.0f, modelYaw, 0.0f));
}

void ThirdPersonCamera::onPause()
{
    core::Input::setMouseMode(core::MouseMode::VISIBLE);
}

void ThirdPersonCamera::onResume()
{
    core::Input::setMouseMode(core::MouseMode::LOCKED);
}

} // namespace siren

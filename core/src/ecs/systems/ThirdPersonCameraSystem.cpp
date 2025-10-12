#include "ThirdPersonCameraSystem.hpp"

#include "core/Input.hpp"
#include "ecs/components/ThirdPersonCameraComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/Scene.hpp"

namespace siren::ecs
{

void ThirdPersonCameraSystem::onReady(ecs::Scene& scene)
{
    core::Input::setMouseMode(core::MouseMode::LOCKED);
}

void ThirdPersonCameraSystem::onShutdown(Scene& scene)
{
    core::Input::setMouseMode(core::MouseMode::VISIBLE);
}

void ThirdPersonCameraSystem::onUpdate(const float delta, Scene& scene)
{
    for (const auto& e : scene.getWith<ThirdPersonCameraComponent, TransformComponent>()) {
        auto& transform = scene.get<TransformComponent>(e);         // the models transform
        auto& camera    = scene.get<ThirdPersonCameraComponent>(e); // the camera

        const glm::vec2 mouseDelta = core::Input::getDeltaMousePosition();
        const float deltaSens      = camera.sensitivity * camera.rotationSpeed;

        camera.yaw -= mouseDelta.x * deltaSens;
        camera.pitch += mouseDelta.y * deltaSens;
        camera.pitch =
            glm::clamp(camera.pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

        glm::vec3 focalPoint = transform.position + camera.focalOffset;

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
}

} // namespace siren::ecs

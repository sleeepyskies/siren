#include "CameraSystem.hpp"

#include <core/Input.hpp>
#include <ecs/Scene.hpp>

namespace siren::ecs
{

void CameraSystem::onReady(secs::Scene& scene)
{
    core::Input::setMouseMode(core::MouseMode::LOCKED);
}

void CameraSystem::onShutdown(secs::Scene& scene)
{
    core::Input::setMouseMode(core::MouseMode::VISIBLE);
}

void CameraSystem::onUpdate(const float delta, secs::Scene& scene)
{
    // query for all entities with Transform
    const auto& entities = scene.getComponentEntities<CameraComponent, TransformComponent>();

    for (const auto& e : entities) {
        auto& tc = scene.getComponent<TransformComponent>(e);
        auto& cc = scene.getComponent<CameraComponent>(e);
        look(delta, tc, cc);
    }
}

void CameraSystem::look(const float delta, TransformComponent& transform,
                        CameraComponent& camera) const
{
    glm::vec2 mouseDelta = core::Input::getDeltaMousePosition();
    float deltaSens      = camera.sensitivity * camera.rotationSpeed;

    // update yaw and pitch
    camera.yaw -= mouseDelta.x * deltaSens;
    camera.pitch -= mouseDelta.y * deltaSens;

    // clamp pitch (e.g., +/- 89 degrees)
    camera.pitch = glm::clamp(camera.pitch, glm::radians(-80.0f), glm::radians(80.0f));

    // rebuild rotation quaternion
    glm::quat qYaw     = glm::angleAxis(camera.yaw, glm::vec3(0, 1, 0));
    glm::quat qPitch   = glm::angleAxis(camera.pitch, glm::vec3(1, 0, 0));
    transform.rotation = glm::normalize(qYaw * qPitch);

    camera.position      = transform.position + camera.modelOffset;
    camera.viewDirection = transform.rotation * glm::vec3(0, 0, -1);
}

} // namespace siren::ecs
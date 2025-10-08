#include "PlayerControllerSystem.hpp"

#include "ecs/components/PlayerComponent.hpp"
#include "ecs/components/TransformComponent.hpp"
#include "ecs/core/Scene.hpp"

#include "core/Input.hpp"

#include <glm/gtc/quaternion.hpp>

namespace siren::ecs
{

void PlayerControllerSystem::onUpdate(const float delta, Scene& scene)
{
    // query for all entities with Transform
    const auto& entities = scene.getWith<PlayerComponent, TransformComponent>();

    for (const auto& e : entities) {
        auto& tc = scene.get<TransformComponent>(e);
        auto& pc = scene.get<PlayerComponent>(e);
        move(delta, tc, pc);
    }
}

void PlayerControllerSystem::move(const float delta, TransformComponent& transformComponent,
                                  const PlayerComponent& playerComponent) const
{
    glm::vec3 dir{}; // use accumulative vector to avoid faster diagonal movement

    if (core::Input::isKeyPressed(core::KeyCode::W)) { dir += glm::vec3(0, 0, 1); }
    if (core::Input::isKeyPressed(core::KeyCode::A)) { dir += glm::vec3(-1, 0, 0); }
    if (core::Input::isKeyPressed(core::KeyCode::S)) { dir += glm::vec3(0, 0, -1); }
    if (core::Input::isKeyPressed(core::KeyCode::D)) { dir += glm::vec3(1, 0, 0); }
    if (core::Input::isKeyPressed(core::KeyCode::SPACE)) { dir += glm::vec3(0, 1, 0); }
    if (core::Input::isKeyPressed(core::KeyCode::L_SHIFT)) { dir += glm::vec3(0, -1, 0); }

    if (glm::length(dir) == 0) { return; }

    dir               = glm::normalize(dir);
    glm::vec3 forward = transformComponent.rotation * glm::vec3(0, 0, -1);
    glm::vec3 right   = transformComponent.rotation * glm::vec3(1, 0, 0);
    glm::vec3 up      = glm::vec3(0, 1, 0);

    glm::vec3 move = dir.x * right + dir.y * up + dir.z * forward;

    transformComponent.position += move * delta * playerComponent.movementSpeed;
}

} // namespace siren::ecs

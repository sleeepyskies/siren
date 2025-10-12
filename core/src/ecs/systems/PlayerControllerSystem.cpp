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
    for (const auto& e : scene.getWith<PlayerComponent, TransformComponent>()) {
        auto& transform = scene.get<TransformComponent>(e);
        auto& player    = scene.get<PlayerComponent>(e);

        glm::vec3 dir{}; // use accumulative vector to avoid faster diagonal movement

        if (core::Input::isKeyPressed(core::KeyCode::W)) { dir += glm::vec3(0, 0, 1); }
        if (core::Input::isKeyPressed(core::KeyCode::A)) { dir += glm::vec3(-1, 0, 0); }
        if (core::Input::isKeyPressed(core::KeyCode::S)) { dir += glm::vec3(0, 0, -1); }
        if (core::Input::isKeyPressed(core::KeyCode::D)) { dir += glm::vec3(1, 0, 0); }
        if (core::Input::isKeyPressed(core::KeyCode::SPACE)) { dir += glm::vec3(0, 1, 0); }
        if (core::Input::isKeyPressed(core::KeyCode::L_SHIFT)) { dir += glm::vec3(0, -1, 0); }

        if (glm::length(dir) == 0) { return; } // no input, can skip all

        dir               = glm::normalize(dir);
        glm::vec3 forward = transform.rotation * glm::vec3(0, 0, -1);
        glm::vec3 right   = transform.rotation * glm::vec3(1, 0, 0);
        glm::vec3 up      = glm::vec3(0, 1, 0);

        glm::vec3 move = dir.x * right + dir.y * up + dir.z * forward;

        transform.position += move * delta * player.movementSpeed;
    }
}

} // namespace siren::ecs

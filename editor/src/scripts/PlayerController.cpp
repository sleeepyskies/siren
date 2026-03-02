#include "PlayerController.hpp"

#include "../../../core/modules/input/input.hpp"
#include "ecs/Components.hpp"

#include <glm/gtc/quaternion.hpp>


namespace siren::editor
{

void PlayerController::onUpdate(const float delta)
{
    const auto& in = core::input();

    auto& transform = get<core::TransformComponent>();

    glm::vec3 dir{ }; // use accumulative vector to avoid faster diagonal movement

    if (in.isKeyPressed(core::Key::W)) { dir += glm::vec3(0, 0, 1); }
    if (in.isKeyPressed(core::Key::A)) { dir += glm::vec3(-1, 0, 0); }
    if (in.isKeyPressed(core::Key::S)) { dir += glm::vec3(0, 0, -1); }
    if (in.isKeyPressed(core::Key::D)) { dir += glm::vec3(1, 0, 0); }
    if (in.isKeyPressed(core::Key::SPACE)) { dir += glm::vec3(0, 1, 0); }
    if (in.isKeyPressed(core::Key::L_SHIFT)) { dir += glm::vec3(0, -1, 0); }

    if (glm::length(dir) == 0) { return; } // no input, can skip all

    dir               = glm::normalize(dir);
    glm::vec3 forward = transform.rotation * glm::vec3(0, 0, -1);
    glm::vec3 right   = transform.rotation * glm::vec3(1, 0, 0);
    glm::vec3 up      = glm::vec3(0, 1, 0);

    glm::vec3 move = dir.x * right + dir.y * up + dir.z * forward;

    transform.translation += move * delta * m_movementSpeed;
}
} // namespace siren

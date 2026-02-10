#include "PlayerController.hpp"

#include "input/InputModule.hpp"
#include "ecs/Components.hpp"

#include <glm/gtc/quaternion.hpp>


namespace siren::editor
{
void PlayerController::onUpdate(const float delta) {
    const auto& input = core::Locator<core::InputModule>::locate();

    auto& transform = get<core::TransformComponent>();

    glm::vec3 dir{ }; // use accumulative vector to avoid faster diagonal movement

    if (input.is_key_held(core::KeyCode::W)) { dir += glm::vec3(0, 0, 1); }
    if (input.is_key_held(core::KeyCode::A)) { dir += glm::vec3(-1, 0, 0); }
    if (input.is_key_held(core::KeyCode::S)) { dir += glm::vec3(0, 0, -1); }
    if (input.is_key_held(core::KeyCode::D)) { dir += glm::vec3(1, 0, 0); }
    if (input.is_key_held(core::KeyCode::SPACE)) { dir += glm::vec3(0, 1, 0); }
    if (input.is_key_held(core::KeyCode::L_SHIFT)) { dir += glm::vec3(0, -1, 0); }

    if (glm::length(dir) == 0) { return; } // no input, can skip all

    glm::mat4 rot{ 1.f };
    rot = glm::rotate(rot, transform.rotation.x, { 1, 0, 0 });
    rot = glm::rotate(rot, transform.rotation.y, { 0, 1, 0 });
    rot = glm::rotate(rot, transform.rotation.z, { 0, 0, 1 });

    dir                = glm::normalize(dir);
    const auto forward = glm::vec3{ rot * glm::vec4(0, 0, -1, 0) };
    const auto right   = glm::vec3{ rot * glm::vec4(1, 0, 0, 0) };
    const auto up      = glm::vec3(0, 1, 0);

    glm::vec3 move = dir.x * right + dir.y * up + dir.z * forward;

    transform.translation += move * delta * m_movementSpeed;
}
} // namespace siren

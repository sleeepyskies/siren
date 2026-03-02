#include "input.hpp"

#include "core/app.hpp"
#include "core/locator.hpp"
#include "core/event_bus.hpp"
#include "core/events.hpp"
#include "core/spch.hpp"


namespace siren::core
{

auto MouseMovement::position() const noexcept -> glm::vec2 {
    return m_current_mouse_positon;
}

auto MouseMovement::mouse_delta() const noexcept -> glm::vec2 {
    return m_mouse_delta;
}

auto MouseMovement::scroll_delta() const noexcept -> glm::vec2 {
    return m_scroll_delta;
}

auto MouseMovement::update() noexcept -> void {
    m_mouse_delta             = m_current_mouse_positon - m_previous_mouse_position;
    m_previous_mouse_position = m_current_mouse_positon;

    m_scroll_delta       = m_accumulated_scroll;
    m_accumulated_scroll = glm::vec2{ 0 };
}

auto MouseMovement::on_scroll(const glm::vec2 value) noexcept -> void {
    m_accumulated_scroll += value;
}

auto MouseMovement::on_mouse_move(const glm::vec2 value) noexcept -> void {
    m_current_mouse_positon = value;
}

Input::Input() {
    auto& event_bus = Locator<EventBus>::value();

    event_bus.subscribe<KeyboardButtonPressedEvent>(
        [this] (const auto& e) {
            m_keyboard_buttons.press(e.key);
        }
    );

    event_bus.subscribe<KeyboardButtonReleasedEvent>(
        [this] (const auto& e) {
            m_keyboard_buttons.release(e.key);
        }
    );

    event_bus.subscribe<MouseButtonPressedEvent>(
        [this] (const auto& e) {
            m_mouse_buttons.press(e.key);
        }
    );

    event_bus.subscribe<MouseButtonReleasedEvent>(
        [this] (const auto& e) {
            m_mouse_buttons.release(e.key);
        }
    );

    event_bus.subscribe<ScrollEvent>(
        [this] (const auto& e) {
            m_mouse_movement.on_scroll(e.delta);
        }
    );

    event_bus.subscribe<MouseMotionEvent>(
        [this] (const auto& e) {
            m_mouse_movement.on_mouse_move(e.position);
        }
    );
}

auto Input::keyboard_buttons() const noexcept -> const ButtonState<Key>& {
    return m_keyboard_buttons;
}

auto Input::mouse_buttons() const noexcept -> const ButtonState<Mouse>& {
    return m_mouse_buttons;
}

auto Input::mouse_movement() const noexcept -> const MouseMovement& {
    return m_mouse_movement;
}

auto Input::update() noexcept -> void {
    m_keyboard_buttons.update();
    m_mouse_buttons.update();
    m_mouse_movement.update();
}

} // namespace siren::core

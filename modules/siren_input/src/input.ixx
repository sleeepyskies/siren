module;

#include <utility>
#include <bitset>

export module siren.input.input;

import siren.common;
import siren.math;

import siren.input.input_codes;

namespace siren::input {

export class Input;

/**
 * @class ButtonState
 * @brief Handles input state for some set of buttons given by Button.
 * @tparam Button The Enum representing the Button type we want to model state for.
 */
export template <IsSizedEnum Button>
class ButtonState {
    friend class Input;

    static constexpr usize Size = std::to_underlying(Button::Max);
    using BitSet                = std::bitset<Size>;

    static constexpr auto to_index(const Button button) noexcept -> usize { return std::to_underlying(button); }

public:
    /**
     * @brief Checks if the given button is currently pressed.
     * @param button The button to check.
     * @return True if the button is pressed, false otherwise.
     */
    [[nodiscard]] auto pressed(const Button button) const noexcept -> bool { return m_pressed.test(to_index(button)); }

    /**
     * @brief Checks if the given button is currently not pressed aka released.
     * @param button The button to check.
     * @return True if the button is not pressed, false otherwise.
     */
    [[nodiscard]] auto released(const Button button) const noexcept -> bool { return !pressed(button); }

    /**
     * @brief Checks if the given button was just pressed this frame.
     * @param button The button to check.
     * @return True if the button was just pressed this frame, false otherwise.
     */
    [[nodiscard]] auto just_pressed(const Button button) const noexcept -> bool {
        return m_just_pressed.test(to_index(button));
    }

    /**
     * @brief Checks if the given button was just released this frame.
     * @param button The button to check.
     * @return True if the button was just released this frame, false otherwise.
     */
    [[nodiscard]] auto just_released(const Button button) const noexcept -> bool {
        return m_just_released.test(to_index(button));
    }

    /**
     * @brief Sets a buttons state as pressed.
     * @param button The button to press.
     * @note If calling this, make sure to also call release() at some point.
     */
    auto press(const Button button) noexcept -> void {
        const auto idx = to_index(button);
        if (!m_pressed.test(idx)) {
            m_pressed.set(idx);
            m_just_pressed.set(idx);
        }
    }

    /**
     * @brief Sets a buttons state as released.
     * @param button The button to press.
     */
    auto release(const Button button) noexcept -> void {
        const auto idx = to_index(button);
        m_pressed.reset(idx);
        m_just_pressed.reset(idx);
        m_just_released.set(idx);
    }

private:
    auto update() noexcept -> void {
        m_just_pressed.reset();
        m_just_released.reset();
    }

    BitSet m_pressed;
    BitSet m_just_pressed;
    BitSet m_just_released;
};

/**
 * @class MouseMovement
 * @brief Handles tracking continuous mouse data.
 */
export class MouseMovement {
public:
    MouseMovement() = default;

    /** @breif Returns the current absolute position of the cursor in screen coordinates. */
    [[nodiscard]] auto position() const noexcept -> glm::vec2;

    /** @breif Returns the relative movement of the cursor since the last frame. */
    [[nodiscard]] auto mouse_delta() const noexcept -> glm::vec2;

    /** @breif Returns the accumulated scroll offset for the current frame. */
    [[nodiscard]] auto scroll_delta() const noexcept -> glm::vec2;

private:
    auto update() noexcept -> void;
    auto on_scroll(glm::vec2 value) noexcept -> void;
    auto on_mouse_move(glm::vec2 value) noexcept -> void;

    friend class Input;
    glm::vec2 m_current_mouse_positon;
    glm::vec2 m_previous_mouse_position;
    glm::vec2 m_mouse_delta;

    glm::vec2 m_scroll_delta;
    glm::vec2 m_accumulated_scroll;
};

/** @brief Simple type alias for accessing mouse input. */
export using MouseInput = ButtonState<Mouse>;

/** @brief Simple type alias for accessing keyboard input. */
export using KeyInput = ButtonState<Key>;

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

} // namespace siren::input

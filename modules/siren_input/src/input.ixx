module;

#include <utility>
#include <bitset>

export module siren.input.input;

import siren.common;
import siren.math;
import siren.ecs.system;

import siren.input.input_codes;

namespace siren::input {

export class InputProxy;

/**
 * @class ButtonState
 * @brief Handles input state for some set of buttons given by Button.
 * @tparam Button The Enum representing the Button type we want to model state for.
 */
export template <IsSizedEnum Button>
class ButtonState {
    friend class InputProxy;

    static constexpr usize Size = std::to_underlying(Button::Max);
    using BitSet                = std::bitset<Size>;

    static constexpr auto to_index(const Button button) noexcept -> usize { return std::to_underlying(button); }

public:
    /**
     * @brief Checks if the given button is currently pressed.
     * @param button The button to check.
     * @return True if the button is pressed, false otherwise.
     */
    [[nodiscard]] auto pressed(Button button) const noexcept -> bool;

    /**
     * @brief Checks if the given button is currently not pressed aka released.
     * @param button The button to check.
     * @return True if the button is not pressed, false otherwise.
     */
    [[nodiscard]] auto released(Button button) const noexcept -> bool;

    /**
     * @brief Checks if the given button was just pressed this frame.
     * @param button The button to check.
     * @return True if the button was just pressed this frame, false otherwise.
     */
    [[nodiscard]] auto just_pressed(Button button) const noexcept -> bool;

    /**
     * @brief Checks if the given button was just released this frame.
     * @param button The button to check.
     * @return True if the button was just released this frame, false otherwise.
     */
    [[nodiscard]] auto just_released(Button button) const noexcept -> bool;

private:
    /**
     * @brief Updates the internal state. Make sure to only call once a frame.
     */
    auto update() noexcept -> void;

    /**
     * @brief Sets a buttons state as pressed.
     * @param button The button to press.
     * @note If calling this, make sure to also call release() at some point.
     */
    auto press(Button button) noexcept -> void;

    /**
     * @brief Sets a buttons state as released.
     * @param button The button to press.
     */
    auto release(Button button) noexcept -> void;

    BitSet m_pressed;
    BitSet m_just_pressed;
    BitSet m_just_released;
};

/**
 * @class MouseMovement
 * @brief Handles tracking continuous mouse data.
 */
export class MouseMovement {
    friend class InputProxy;

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

/**
 * @brief For engine use only utility class for mutating input state.
 */
class InputProxy {
public:
    /** @brief Handles updating all input related resources. */
    static auto update(
        ecs::Resource<MouseInput&> mouse_input,
        ecs::Resource<KeyInput&> key_input,
        ecs::Resource<MouseMovement&> mouse_movement
    ) -> void;

    /** @brief Updates the @ref MouseMovement scroll. */
    static auto on_mouse_scroll(
        MouseMovement& mouse_movement,
        glm::vec2 value
    ) -> void;

    /** @brief Updates the @ref MouseMovement mouse position. */
    static auto on_mouse_move(
        MouseMovement& mouse_movement,
        glm::vec2 position
    ) -> void;

    /** @brief Presses a button. */
    template <IsSizedEnum Button>
    static auto press(ButtonState<Button>& state, Button button) -> void;

    /** @brief Releases a button. */
    template <IsSizedEnum Button>
    static auto release(ButtonState<Button>& state, Button button) -> void;
};

// ============================================================================
// == MARK: ButtonState impl
// ============================================================================

template <IsSizedEnum Button>
auto ButtonState<Button>::pressed(const Button button) const noexcept -> bool {
    return m_pressed.test(to_index(button));
}

template <IsSizedEnum Button>
auto ButtonState<Button>::released(const Button button) const noexcept -> bool {
    return !pressed(button);
}

template <IsSizedEnum Button>
auto ButtonState<Button>::just_pressed(const Button button) const noexcept -> bool {
    return m_just_pressed.test(to_index(button));
}

template <IsSizedEnum Button>
auto ButtonState<Button>::just_released(const Button button) const noexcept -> bool {
    return m_just_released.test(to_index(button));
}

template <IsSizedEnum Button>
auto ButtonState<Button>::update() noexcept -> void {
    m_just_pressed.reset();
    m_just_released.reset();
}

template <IsSizedEnum Button>
auto ButtonState<Button>::press(const Button button) noexcept -> void {
    const auto idx = to_index(button);
    if (!m_pressed.test(idx)) {
        m_pressed.set(idx);
        m_just_pressed.set(idx);
    }
}

template <IsSizedEnum Button>
auto ButtonState<Button>::release(const Button button) noexcept -> void {
    const auto idx = to_index(button);
    m_pressed.reset(idx);
    m_just_pressed.reset(idx);
    m_just_released.set(idx);
}

// ============================================================================
// == MARK: MouseMovement impl
// ============================================================================

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

// ============================================================================
// == MARK: InputProxy impl
// ============================================================================

auto InputProxy::update(
    ecs::Resource<MouseInput&> mouse_input,
    ecs::Resource<KeyInput&> key_input,
    ecs::Resource<MouseMovement&> mouse_movement
) -> void {
    mouse_input->update();
    key_input->update();
    mouse_movement->update();
}

auto InputProxy::on_mouse_scroll(MouseMovement& mouse_movement, const glm::vec2 value) -> void {
    mouse_movement.m_accumulated_scroll += value;
}

auto InputProxy::on_mouse_move(MouseMovement& mouse_movement, const glm::vec2 position) -> void {
    mouse_movement.m_current_mouse_positon = position;
}

template <IsSizedEnum Button>
auto InputProxy::press(ButtonState<Button>& state, const Button button) -> void {
    state.press(button);
}

template <IsSizedEnum Button>
auto InputProxy::release(ButtonState<Button>& state, const Button button) -> void {
    state.release(button);
}

} // namespace siren::input

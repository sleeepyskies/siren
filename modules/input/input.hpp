#pragma once

#include "input_codes.hpp"
#include "core/spch.hpp"


namespace siren::core
{

/**
 * @class ButtonState
 * @brief Handles input state for some set of buttons given by Button.
 * @tparam Button The Enum representing the Button type we want to model state for.
 */
template <IsSizedEnum Button>
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
     * @return True if the button was just relased this frame, false otherwise.
     */
    [[nodiscard]] auto just_released(const Button button) const noexcept -> bool {
        return m_just_released.test(to_index(button));
    }

private:
    auto press(const Button button) noexcept -> void {
        const auto idx = to_index(button);
        if (!m_pressed.test(idx)) {
            m_pressed.set(idx);
            m_just_pressed.set(idx);
        }
    }

    auto release(const Button button) noexcept -> void {
        const auto idx = to_index(button);
        m_pressed.reset(idx);
        m_just_pressed.reset(idx);
        m_just_released.set(idx);
    }

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
class MouseMovement {
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

class Input final {
public:
    Input();
    ~Input() = default;

    /**
     * @brief Provides access to keyboard button state.
     * @see ButtonState
     * @see Key
     */
    [[nodiscard]] auto keyboard_buttons() const noexcept -> const ButtonState<Key>&;

    /**
     * @brief Provides access to mouse button state.
     * @see ButtonState
     * @see Mouse
     */
    [[nodiscard]] auto mouse_buttons() const noexcept -> const ButtonState<Mouse>&;

    /**
     * @brief Provides access to mouse movement state.
     * @see MouseMovement
     */
    [[nodiscard]] auto mouse_movement() const noexcept -> const MouseMovement&;

private:
    friend class App;

    auto update() noexcept -> void;

    ButtonState<Key> m_keyboard_buttons;
    ButtonState<Mouse> m_mouse_buttons;
    MouseMovement m_mouse_movement{ };
};

} // namespace siren::core

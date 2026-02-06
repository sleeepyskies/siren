#include "InputModule.hpp"

#include "core/App.hpp"
#include "core/Locator.hpp"

#include "events/EventBus.hpp"
#include "events/Events.hpp"

#include "utilities/spch.hpp"


namespace siren::core
{
InputModule::InputModule() {
    auto& event_bus = Locator<EventBus>::locate();

    event_bus.subscribe<KeyPressedEvent>(
        [this] (auto& event) {
            m_keys[static_cast<i32>(event.key)] = true;
            return false;
        }
    );

    event_bus.subscribe<KeyReleasedEvent>(
        [this] (auto& event) {
            m_keys[static_cast<i32>(event.key)] = false;
            return false;
        }
    );

    event_bus.subscribe<MouseKeyPressedEvent>(
        [this] (auto& event) {
            m_mousekeys[static_cast<i32>(event.key)] = true;
            return false;
        }
    );

    event_bus.subscribe<MouseKeyReleasedEvent>(
        [this] (auto& event) {
            m_mousekeys[static_cast<i32>(event.key)] = false;
            return false;
        }
    );

    event_bus.subscribe<ScrollEvent>(
        [this] (auto& event) {
            m_scroll_offset = glm::vec2{ event.xOffset, event.yOffset };
            return false;
        }
    );

    event_bus.subscribe<MouseMovedEvent>(
        [this] (auto& event) {
            m_current_mouse_position = glm::vec2{ event.x, event.y };
            return false;
        }
    );
}

void InputModule::update() {
    m_scroll_offset           = glm::vec2(0.f);
    m_previous_mouse_position = m_current_mouse_position;

    m_previous_keys      = m_keys;
    m_previous_mousekeys = m_mousekeys;
}

bool InputModule::is_key_pressed(const KeyCode code) const {
    return m_keys[static_cast<size_t>(code)] && !m_previous_keys[static_cast<size_t>(code)];
}

bool InputModule::is_key_held(const KeyCode code) const {
    return m_keys[static_cast<size_t>(code)];
}

bool InputModule::is_key_released(const KeyCode code) const {
    return !m_keys[static_cast<size_t>(code)] && m_previous_keys[static_cast<size_t>(code)];
}

bool InputModule::is_mouse_key_pressed(MouseCode code) const {
    return m_mousekeys[static_cast<size_t>(code)] && !m_previous_mousekeys[static_cast<size_t>(code)];
}

bool InputModule::is_mouse_key_held(const MouseCode code) const {
    return m_mousekeys[static_cast<size_t>(code)];
}

bool InputModule::is_mouse_key_released(MouseCode code) const {
    return !m_mousekeys[static_cast<size_t>(code)] && m_previous_mousekeys[static_cast<size_t>(code)];
}

glm::vec2 InputModule::mouse_position() const {
    return m_current_mouse_position;
}

glm::vec2 InputModule::delta_mouse_position() const {
    return m_current_mouse_position - m_previous_mouse_position;
}

glm::vec2 InputModule::scroll_delta() const {
    return m_scroll_offset;
}
} // namespace siren::core

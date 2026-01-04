#include "InputModule.hpp"

#include "core/App.hpp"

#include "events/Events.hpp"

#include "platform/windows/WindowsInput.hpp"
#include "utilities/spch.hpp"
#include "window/WindowModule.hpp"


namespace siren::core
{
bool InputModule::Init()
{
    App::Get().GetEventBus().Subscribe<KeyPressedEvent>(
        [this] (auto& event) {
            m_keys[static_cast<i32>(event.key)] = true;
            return false;
        }
    );

    App::Get().GetEventBus().Subscribe<KeyReleasedEvent>(
        [this] (auto& event) {
            m_keys[static_cast<i32>(event.key)] = false;
            return false;
        }
    );

    App::Get().GetEventBus().Subscribe<MouseKeyPressedEvent>(
        [this] (auto& event) {
            m_mouseKeys[static_cast<i32>(event.key)] = true;
            return false;
        }
    );

    App::Get().GetEventBus().Subscribe<MouseKeyReleasedEvent>(
        [this] (auto& event) {
            m_mouseKeys[static_cast<i32>(event.key)] = false;
            return false;
        }
    );

    App::Get().GetEventBus().Subscribe<ScrollEvent>(
        [this] (auto& event) {
            m_scrollOffset = glm::vec2{ event.xOffset, event.yOffset };
            return false;
        }
    );

    App::Get().GetEventBus().Subscribe<MouseMovedEvent>(
        [this] (auto& event) {
            m_currentMousePosition = glm::vec2{ event.x, event.y };
            return false;
        }
    );

    return true;
}

void InputModule::update()
{
    m_scrollOffset          = glm::vec2(0.f);
    m_previousMousePosition = m_currentMousePosition;

    m_previousKeys      = m_keys;
    m_previousMouseKeys = m_mouseKeys;
}

bool InputModule::isKeyPressed(const KeyCode code) const
{
    return m_keys[static_cast<size_t>(code)] && !m_previousKeys[static_cast<size_t>(code)];
}

bool InputModule::isKeyHeld(const KeyCode code) const
{
    return m_keys[static_cast<size_t>(code)];
}

bool InputModule::isKeyReleased(const KeyCode code) const
{
    return !m_keys[static_cast<size_t>(code)] && m_previousKeys[static_cast<size_t>(code)];
}

bool InputModule::isMouseKeyPressed(MouseCode code) const
{
    return m_mouseKeys[static_cast<size_t>(code)] && !m_previousMouseKeys[static_cast<size_t>(code)];
}

bool InputModule::isMouseKeyHeld(const MouseCode code) const
{
    return m_mouseKeys[static_cast<size_t>(code)];
}

bool InputModule::isMouseKeyReleased(MouseCode code) const
{
    return !m_mouseKeys[static_cast<size_t>(code)] && m_previousMouseKeys[static_cast<size_t>(code)];
}

glm::vec2 InputModule::getMousePosition() const
{
    return m_currentMousePosition;
}

glm::vec2 InputModule::getDeltaMousePosition() const
{
    return m_currentMousePosition - m_previousMousePosition;
}

glm::vec2 InputModule::getScrollDelta() const
{
    return m_scrollOffset;
}
} // namespace siren::core

#include "InputModule.hpp"

#include "core/App.hpp"
#include "platform/windows/WindowsInput.hpp"
#include "utilities/spch.hpp"
#include "window/WindowModule.hpp"


namespace siren::core
{
bool InputModule::initialize()
{
    switch (app().getProperties().OS) {
        case App::Properties::OS::NONE: return false;
        case App::Properties::OS::WINDOWS: {
            m_input =
                    createOwn<platform::WindowsInput>(static_cast<GLFWwindow*>(window().handle()));
            window().setScrollCallback(
                [this] (const glm::vec2 scroll) { this->m_scrollOffset = scroll; }
            );
            return true;
        }
    }

    return false;
}

void InputModule::update()
{
    m_scrollOffset          = glm::vec2(0.f);
    m_previousMousePosition = m_currentMousePosition;
    m_currentMousePosition  = m_input->getMousePosition();

    m_previousKeys = m_keys;
    for (size_t key = 0; key < static_cast<size_t>(KeyCode::MAX); key++) {
        m_keys[key] = m_input->isKeyHeld(static_cast<KeyCode>(key));
    }

    m_previousMouseKeys = m_mouseKeys;
    for (size_t mouseKey = 0; mouseKey < static_cast<size_t>(MouseCode::MAX); mouseKey++) {
        m_mouseKeys[mouseKey] = m_input->isMouseKeyHeld(static_cast<MouseCode>(mouseKey));
    }
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
    return m_input->getMousePosition();
}

void InputModule::setMousePosition(const glm::vec2 position) const
{
    return m_input->setMousePosition(position);
}

glm::vec2 InputModule::getDeltaMousePosition() const
{
    return m_previousMousePosition - getMousePosition();
}

MouseMode InputModule::getMouseMode() const
{
    return m_input->getMouseMode();
}

void InputModule::setMouseMode(const MouseMode mode) const
{
    return m_input->setMouseMode(mode);
}

glm::vec2 InputModule::getScrollDelta() const
{
    return m_scrollOffset;
}
} // namespace siren::core

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
        case App::Properties::OS::NONE   : return false;
        case App::Properties::OS::WINDOWS: {
            m_input =
                createOwn<platform::WindowsInput>(static_cast<GLFWwindow*>(window().handle()));
            window().setScrollCallback(
                [this](const glm::vec2 scroll) { this->m_scrollOffset = scroll; });
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
}

bool InputModule::isKeyPressed(const KeyCode code) const
{
    return m_input->isKeyPressed(code);
}

bool InputModule::isMouseKeyPressed(const MouseCode code) const
{
    return m_input->isMouseKeyPressed(code);
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

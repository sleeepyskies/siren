#include "WindowsTimer.hpp"

#include "platform/GL.hpp"

namespace siren::platform
{

float WindowsTimer::getTimeSeconds() const
{
    return glfwGetTime();
}

float WindowsTimer::getTimeMilliseconds() const
{
    return glfwGetTime() * 1000;
}

} // namespace siren::platform
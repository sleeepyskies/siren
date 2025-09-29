#include "Time.hpp"

#include "core/GL.hpp"

namespace core
{

float Time::delta()
{
    const float elapsedF = elapsed();
    const float delta    = elapsedF - m_previousFrame;
    m_previousFrame      = elapsedF;

    return delta;
}

float Time::elapsed() const
{
    return glfwGetTime();
}

} // namespace core

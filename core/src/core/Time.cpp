#include "Time.hpp"

#include "core/GL.hpp"

namespace siren::core
{

float Time::delta()
{
    const float elapsedF = elapsed();
    const float delta    = elapsedF - m_previousFrame;
    m_previousFrame      = elapsedF;

    // clamp to lowest of 1 to avoid strange behaviour for frame drops
    return std::min(delta, 1.f);
}

float Time::elapsed() const
{
    return glfwGetTime();
}

} // namespace siren::core

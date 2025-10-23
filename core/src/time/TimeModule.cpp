#include "TimeModule.hpp"

#include "core/App.hpp"
#include "core/Core.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

bool TimeModule::initialize()
{
    switch (app().getProperties().OS) {
        case App::Properties::OS::NONE: {
            err("Cannot setup TimeModule when OS is NONE.");
            return false;
        }
        case App::Properties::OS::WINDOWS: {
            m_timer = createOwn<platform::WindowsTimer>();
            return true;
        }
    }
}

float TimeModule::delta()
{
    const float elapsedF = elapsed();
    const float delta    = elapsedF - m_previousFrame;
    m_previousFrame      = elapsedF;

    // clamp to lowest of 1 to avoid strange behaviour for frame drops
    return std::min(delta, 1.f);
}

float TimeModule::elapsed() const
{
    return m_timer->getTimeSeconds();
}

const char* TimeModule::getName()
{
    return "TimeModule";
}

} // namespace siren::core

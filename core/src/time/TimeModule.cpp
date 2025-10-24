#include "TimeModule.hpp"

#include "core/App.hpp"
#include "platform/windows/WindowsTimer.hpp"
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

    return false;
}

float TimeModule::delta()
{
    const float elapsedF = elapsedS();
    const float delta    = elapsedF - m_previousFrame;
    m_previousFrame      = elapsedF;

    // clamp to lowest of 1 to avoid strange behaviour for frame drops
    return std::min(delta, 1.f);
}

float TimeModule::elapsedS() const
{
    return m_timer->getTimeSeconds();
}

float TimeModule::elapsedMs() const
{
    return m_timer->getTimeMilliseconds();
}

const char* TimeModule::getName()
{
    return "TimeModule";
}

} // namespace siren::core

#include "Timer.hpp"

#include <slog.hpp>


namespace siren::core
{

static std::chrono::steady_clock::time_point s_start;
static std::chrono::steady_clock::time_point s_frameStart;
static std::chrono::steady_clock::time_point s_previousFrame;
static float s_deltaMs = 0;


void Timer::initialize()
{
    s_start         = std::chrono::steady_clock::now();
    s_frameStart    = s_start;
    s_previousFrame = s_start;
    nfo("Timer initialized");
}

void Timer::tick()
{
    s_previousFrame = s_frameStart;
    s_frameStart    = std::chrono::steady_clock::now();
    s_deltaMs       = std::chrono::duration<float, std::milli>(s_frameStart - s_previousFrame).count();
}

float Timer::getElapsedS()
{
    return getElapsedMs() / 1000;
}

float Timer::getElapsedMs()
{
    return std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - s_start).count();
}

float Timer::getDelta()
{
    return s_deltaMs / 1000;
}

float Timer::getDeltaMS()
{
    return s_deltaMs;
}

} // siren::core

#include "Timer.hpp"

#include <slog.hpp>


namespace siren::core
{
static std::chrono::steady_clock::time_point s_start;
static std::chrono::steady_clock::time_point s_frameStart;
static std::chrono::steady_clock::time_point s_previousFrame;
static float s_deltaMs = 0;

void Timer::init()
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

float Timer::get_elapsed()
{
    return get_elapsed_ms() / 1000;
}

float Timer::get_elapsed_ms()
{
    return std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - s_start).count();
}

float Timer::get_delta()
{
    return s_deltaMs / 1000;
}

float Timer::get_delta_ms()
{
    return s_deltaMs;
}
} // siren::core

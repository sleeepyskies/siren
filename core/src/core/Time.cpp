#include "Time.hpp"


namespace siren::core
{
static std::chrono::steady_clock::time_point s_start;
static std::chrono::steady_clock::time_point s_frameStart;
static std::chrono::steady_clock::time_point s_previousFrame;
static f32 s_deltaMs = 0;

void Time::init() {
    s_start         = std::chrono::steady_clock::now();
    s_frameStart    = s_start;
    s_previousFrame = s_start;
}

void Time::tick() {
    s_previousFrame = s_frameStart;
    s_frameStart    = std::chrono::steady_clock::now();
    s_deltaMs       = std::chrono::duration<f32, std::milli>(s_frameStart - s_previousFrame).count();
}

f32 Time::get_elapsed() {
    return get_elapsed_ms() / 1000;
}

f32 Time::get_elapsed_ms() {
    return std::chrono::duration<f32, std::milli>(std::chrono::steady_clock::now() - s_start).count();
}

f32 Time::get_delta() {
    return s_deltaMs / 1000;
}

f32 Time::get_delta_ms() {
    return s_deltaMs;
}
} // siren::core

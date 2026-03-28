#include "time.hpp"


namespace siren::core {
static std::chrono::steady_clock::time_point s_start;
static std::chrono::steady_clock::time_point s_frame_start;
static std::chrono::steady_clock::time_point s_previous_frame;
static f32 s_delta_ms = 0;

auto Time::init() -> void {
    s_start          = std::chrono::steady_clock::now();
    s_frame_start    = s_start;
    s_previous_frame = s_start;
}

auto Time::tick() -> void {
    s_previous_frame = s_frame_start;
    s_frame_start    = std::chrono::steady_clock::now();
    s_delta_ms       = std::chrono::duration<f32, std::milli>(s_frame_start - s_previous_frame).count();
}

auto Time::get_elapsed() -> f32 {
    return get_elapsed_ms() / 1000;
}

auto Time::get_elapsed_ms() -> f32 {
    return std::chrono::duration<f32, std::milli>(std::chrono::steady_clock::now() - s_start).count();
}

auto Time::delta() -> f32 {
    return s_delta_ms / 1000;
}

auto Time::get_delta_ms() -> f32 {
    return s_delta_ms;
}
} // siren::core

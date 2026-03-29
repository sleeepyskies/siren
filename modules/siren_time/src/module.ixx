module;

#include <chrono>

export module siren.time;

import siren.app.plugin;
import siren.app.app;
import siren.schedule;
import siren.common;
import siren.ecs.system;

namespace siren::time {

namespace chrono = std::chrono;

export class TimePlugin;

/**
* @brief Component containing the time elapsed since the previous frame.
*/
export struct DeltaTime {
    /** @brief The time elapsed since the last frame in seconds. */
    f32 seconds;
    /** @brief The time elapsed since the last frame in milliseconds. */
    f32 millis;
};

/**
* @brief Component containing the time elapsed since engine start.
*/
export struct ElapsedTime {
    /** @brief The time elapsed since engine start in seconds. */
    f32 seconds;
};

/**
 * @brief Private helper resource used within the Time module.
 */
struct TimeState {
    /** @brief The start point of the engine. */
    chrono::time_point<chrono::steady_clock> start;
    /** @brief The start point of the last frame. */
    chrono::time_point<chrono::steady_clock> last_frame;
};

/**
 * @brief Updates any time related resources of the time module.
 * @param delta_time The delta time resource to update.
 * @param elapsed_time The elapsed time resource to update.
 * @param time_state Helper resource to store some useful state.
 */
auto tick_time(
    ecs::Resource<DeltaTime&> delta_time,
    ecs::Resource<ElapsedTime&> elapsed_time,
    ecs::Resource<TimeState&> time_state
) -> void {
    const auto now       = chrono::steady_clock::now();
    const auto delta_new = chrono::duration<f32>(now - time_state->last_frame);

    delta_time->seconds = delta_new.count();
    delta_time->millis  = delta_new.count() * 1000;

    elapsed_time->seconds = chrono::duration<f32>(now - time_state->start).count();

    time_state->last_frame = now;
}

/**
* @brief Initializes any time related resources.
* @param time_state The time state resource to update.
*/
auto init_time(
    ecs::Resource<TimeState&> time_state
) -> void {
    const auto now = chrono::steady_clock::now();

    time_state->start      = now;
    time_state->last_frame = now;
}

/**
 * @class TimePlugin
 * @brief Initializes the time plugin of siren.
 */
class TimePlugin final : Plugin {
public:
    explicit TimePlugin() = default;

    auto construct(App& app) const -> void {
        app
               .add_resource<DeltaTime>()
               .add_resource<ElapsedTime>()
               .add_system(schedule::SchedulePhase::OnStart, init_time)
               .add_system(schedule::SchedulePhase::First, tick_time);
    }

    auto shutdown(App& app) const -> void {
        app.remove_resources<DeltaTime, ElapsedTime>();
    }
};

} // namespace siren::time


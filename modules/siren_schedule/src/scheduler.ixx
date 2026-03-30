module;

#include <array>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

export module siren.schedule.scheduler;

import siren.common;
import siren.ecs.world;

namespace siren::schedule {

/// @todo: Type based schedule?

/**
 * @brief Lists all possible phases systems can be assigned to.
 * The scheduler will run all of these systems at least once, or in a loop.
 * @note The order of systems with a phase is undefined and should not be relied on.
 */
export enum class SchedulePhase {
    /// @brief Run once by the @ref App when starting the main loop.
    OnStart,

    /// @brief The first stage of each step. Runs before every other phase.
    First,
    /// @brief Runs before @ref SchedulePhase::Update.
    PreUpdate,
    /// @brief Runs after SchedulePhase::PreUpdate and before @ref SchedulePhase::PostUpdate.
    Update,
    /// @brief Runs after the SchedulePhase::Update phase and before the SchedulePhase::Render phase.
    PostUpdate,
    /// @brief The third phase of each step. Runs after
    /// SchedulePhase::PostUpdate.
    Render,

    /// @brief Runs once after the main loop has finished.
    OnEnd,

    Max,
};

/// @brief Type erased system. When called, will handle invoking the inner
/// system with the correct arguments.
using SystemErased = std::function<void(ecs::World&)>;
/// @brief Vector of type erased systems.
using SystemErasedList = std::vector<SystemErased>;

/**
 * @brief Simple struct containing all systems for a phase to be run on
 * both the main thread and not the main thread.
 */
struct SystemBucket {
    SystemErasedList main;
    SystemErasedList non_main;
};

/// @brief Mapping of SchedulePhase to a list of systems.
using Systems = std::array<SystemBucket, std::to_underlying(SchedulePhase::Max)>;

/**
 * @brief Ensures a given type is a usable system by the siren scheduler.
 * @tparam Sys The type to check.
 */
export template <typename Sys>
concept IsSystem = requires (const Sys& system) {
    /// @todo: we want to flesh this out more. assert a system only takes Query<> and Resource<> as params
    std::is_invocable_v<Sys>;
};

export class Scheduler {
public:
    auto step(ecs::World& world) const -> void {
        run_phase(SchedulePhase::First, world);
        run_phase(SchedulePhase::PreUpdate, world);
        run_phase(SchedulePhase::Update, world);
        run_phase(SchedulePhase::PostUpdate, world);
        run_phase(SchedulePhase::Render, world);
    }

    /**
     * @todo Make this run things in parallel!
     * @brief Runs a single schedule phase.
     * @note This function should not really be called anywhere but from
     * within the scheduler or the app.
     * @param schedule_phase The phase to run.
     * @param world The @ref siren::ecs::World to run the schedule phase on.
     */
    auto run_phase(const SchedulePhase schedule_phase, ecs::World& world) const -> void {
        for (const SystemBucket& bucket : m_systems[std::to_underlying(schedule_phase)]) {
            for (const auto& system : bucket.main) {
                system(world);
            }

            for (const auto& system : bucket.non_main) {
                system(world);
            }
        }
    }

    /**
     * @brief Registers a new system to be run by the scheduler.
     * @tparam Sys The type of the system to register.
     * @param schedule_phase The phase to register the system into.
     * @param system The specific function instance.
     * @param main_thread Set to true if the system should be only run from the main thread.
     */
    template <typename Sys>
        requires(IsSystem<Sys>)
    auto add_system(const SchedulePhase schedule_phase, Sys&& system, const bool main_thread) -> void {
        using Traits = FunctionTraits<std::decay_t<Sys>>;
        using Args   = Traits::Args;

        auto wrapper = [system = std::move(system)] (ecs::World& world) {
            auto invoke = [&]<typename... P> (TypePack<P...>) {
                system(world.resolve<P>()...);
            };

            invoke(Args{ });
        };

        if (main_thread) {
            m_systems[std::to_underlying(schedule_phase)].main.emplace_back(std::move(wrapper));
        } else {
            m_systems[std::to_underlying(schedule_phase)].non_main.emplace_back(std::move(wrapper));
        }
    }

private:
    Systems m_systems;
};

} // namespace siren::schedule

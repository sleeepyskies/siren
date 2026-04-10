module;

#include <array>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

export module siren.ecs:scheduler;

import siren.common;
import :world;

namespace siren::ecs {

/// @todo: Type based schedule?

/** @brief Identifier types for systems. */
using SystemID = const void*;

/** @brief Returns the @ref SystemID for a given function. */
template <IsCallable Function>
auto system_id(Function&& fn) -> SystemID { return reinterpret_cast<SystemID>(fn); }

export class SystemOrder {
public:
    struct Dependency {
        enum Type { Before, After } type;

        SystemID system_id;
    };

    template <typename System>
        requires(IsCallable<System>)
    SystemOrder(System system) : m_fn_ptr(system) { }

    template <IsCallable Dep>
    auto before(Dep&& dep) -> SystemOrder& {
        m_dependencies.push_back(Dependency{ Dependency::Before, system_id(std::forward<Dep>(dep)) });
        return *this;
    }

    template <IsCallable Dep>
    auto after(Dep&& dep) -> SystemOrder& {
        m_dependencies.push_back(Dependency{ Dependency::After, system_id(std::forward<Dep>(dep)) });
        return *this;
    }

private:
    std::vector<Dependency> m_dependencies;
    const void* m_fn_ptr;
};

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


export {
    /** @brief Tag */
    struct Phase { };

    /** @brief  */
    struct OnStart final : Phase { };

    /** @brief  */
    struct First final : Phase { };

    /** @brief  */
    struct PreUpdate final : Phase { };

    /** @brief  */
    struct Update final : Phase { };

    /** @brief  */
    struct PostUpdate final : Phase { };

    /** @brief  */
    struct Render final : Phase { };

    /** @brief  */
    struct OnEnd final : Phase { };
}


/// @brief Type erased system. When called, will handle invoking the inner
/// system with the correct arguments.
using SystemInvoker = std::function<void(ecs::World&)>;

/**
 * @brief Simple struct containing all systems for a phase to be run on
 * both the main thread and not the main thread.
 */
struct SystemBucket {
    /** @brief Systems that must be run on the main thread. */
    std::vector<SystemInvoker> main;
    /** @brief Systems that must can be run from any thread. */
    std::vector<SystemInvoker> non_main;
};

/// @brief Mapping of SchedulePhase to a list of systems.
// using SystemSchedule = std::array<SystemBucket, std::to_underlying(SchedulePhase::Max)>;

class SystemSchedule {
public:
private:
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
        requires(IsCallable<Sys>)
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
    SystemSchedule m_systems;

};

} // namespace siren::schedule

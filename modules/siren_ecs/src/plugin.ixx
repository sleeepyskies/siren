export module plugin;

import siren.ecs.signal;
import siren.ecs.event;
import siren.ecs.system;

import siren.schedule.scheduler;
import siren.app.plugin;
import siren.app.app;
import siren.log;

namespace siren::ecs {

/** @brief Makes sure we swap all event buffers once a frame. */
auto handle_swap_events(const Resource<EventBus&> event_bus) -> void {
    event_bus->swap_event_buffers();
}

/**
 * @class EcsPlugin
 * @brief Initializes the siren ecs plugin resources and systems.
 */
class EcsPlugin final : Plugin {
public:
    auto construct(App& app) const -> void override {
        app
               .add_resources<SignalBus, EventBus>()
               .add_system(schedule::SchedulePhase::First, handle_swap_events);

        log::info("Siren ECS has been initialized.");
    }

    auto shutdown(App& app) const -> void override {
        app.remove_resources<SignalBus, EventBus>();
    }
};

} // namespace siren::ecs

module;

#include <utility>
#include <functional>
#include <ostream>
#include <memory>
#include <vector>
#include <algorithm>

export module siren.app;

import siren.ecs;
import siren.reflect;
import siren.log;

namespace siren {

/** @brief Makes sure we swap all event buffers once a frame. */
auto handle_swap_events(const ecs::Resource<ecs::EventBus&> event_bus) -> void {
    event_bus->swap_event_buffers();
}

// forward declarations
export class App;

/**
 * @brief Simple struct used as a resource within the @ref siren::ecs::World
 * to control application lifetime.
 */
export struct AppLifetime {
    /**
     * @brief Controls application lifetime. Once set to true,
     * the application will shut down after the current step.
     */
    bool should_exit = false;
};

/**
 * @class Plugin
 * @brief Interface for defining custom extensions to the siren engine.
 * Plugins may modify the @ref siren::App by adding resources and systems.
 */
export class Plugin {
public:
    virtual ~Plugin() = default;

    /**
     * @brief Adds this plugin to the given @ref siren::App.
     * @param app The application to add the plugin to.
     */
    virtual auto construct(App& app) const -> void = 0;

    /**
     * @brief Removes this plugin from the given @ref siren::App.
     * @param app The application to remove the plugin from.
     */
    virtual auto shutdown(App& app) const -> void = 0;
};

/** @brief Ensures the provided TPlugin inherits from Plugin. */
export template <typename TPlugin>
concept IsPlugin = std::derived_from<TPlugin, Plugin>;

/// @todo: a way to remove systems would b nice maybe
/// @todo: topo graph for systems and phases needs to be done.

/**
 * @brief The main application class of siren.
 * Siren uses a moduler plugin pattern for constructing apps.
 * To register new plugins, call App::add_plugin(). This will handle
 * initializing the plugin for you.
 * To start the application, simply call App::run().
 * The App uses a builder pattern for each of use.
 */
class App {
    using Plugins = std::vector<std::unique_ptr<Plugin>>;

public:
    /**
     * @brief The main loop function type. Can be set by the user.
     * A basic example may be:
     * @code
     * auto loop = [](App& app){
     *     app.scheduler().run_phase(schedule::SchedulePhase::Start, app.world());
     *
     *     while (app.world().resource<AppLifetime>().should_exit == true) {
     *         app.scheduler().step(app.world());
     *     }
     *
     *     app.scheduler().run_phase(schedule::SchedulePhase::End, app.world());
     * };
     * @endcode
     */
    using MainLoop = std::function<void(App&)>;

    /** @brief Constructs a siren application. */
    App() {
        log::init();

        this
              ->add_resource<AppLifetime>()
               .add_resource<ecs::SignalBus>(world())
               .add_resource<ecs::EventBus>()
               .add_system(schedule::SchedulePhase::First, ecs::handle_swap_events);
    }

    /**
     * @brief Destructor of the application. Shuts down all plugins in
     * reverse order of registration.
     */
    ~App() { for (auto& plugin : std::ranges::reverse(m_plugins)) { plugin->shutdown(*this); } }

    App(const App&)            = delete;
    App(App&&)                 = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&)      = delete;

    /**
     * @brief Runs the main loop of the application.
     */
    auto run() -> void { m_loop(*this); }

    /**
     * @brief Adds and registers a new plugin with the application.
     * @note If adding a plugin twice, any attempts after the first will
     * have no effect.
     * @tparam TPlugin The plugin to register
     * @param plugin The plugin instance to register. Will default construct
     * an instance if none is provided.
     * @return A reference to this app for the builder pattern.
     */
    template <typename TPlugin>
        requires std::derived_from<TPlugin, Plugin>
    auto add_plugin(TPlugin&& plugin = { }) -> App& {
        if (!has_plugin<TPlugin>()) {
            auto plugin_ptr = std::make_unique<TPlugin>(std::forward<TPlugin>(plugin));
            plugin->construct(*this);
            m_plugins.emplace_back(std::move(plugin_ptr));
        } else {
            log::warn(
                "Plugin {} has already been added, cannot add a plugin twice.", TypeName<TPlugin>::value()
            );
        }
        return *this;
    }

    /**
     * @brief Checks whether the given plugin has already been registered.
     * @tparam TPlugin The plugin to check.
     * @return true if the plugin has been added, false otherwise.
     */
    template <typename TPlugin>
        requires std::derived_from<TPlugin, Plugin>
    auto has_plugin() -> bool {
        return std::ranges::any_of(
            m_plugins, [] (const auto& plugin) { return dynamic_cast<TPlugin*>(plugin.get()) != nullptr; }
        );
    }

    /**
     * @brief Overwrites the main loop of the application.
     * @param loop The new main loop of the application.
     * @return A reference to this app for the builder pattern.
     */
    auto set_loop(MainLoop&& loop) -> App& {
        m_loop = loop;
        return *this;
    }

    /**
     * @brief Registers a new system into the application.
     * @tparam Sys The type of the system to register.
     * @param schedule_phase The phase to register the system into.
     * @param system The specific function instance. Default constructs
     * an instance if none is provided.
     * @param main_thread If the system should only be run from the main thread.
     * @return A reference to this app for the builder pattern.
     */
    template <typename Sys>
        requires(IsCallable<Sys>)
    auto add_system(
        const ecs::SchedulePhase schedule_phase,
        Sys&& system           = { },
        const bool main_thread = false
    ) -> App& {
        m_scheduler.add_system(schedule_phase, std::forward<Sys>(system), main_thread);
        return *this;
    }

    /**
     * @brief Returns the @ref ecs::World of the application.
     * @return The @ref ecs::World of the application.
     */
    template <typename Self>
    auto world(this Self&& self) noexcept -> auto&& { return std::forward<Self>(self).m_world; }

    /**
     * @brief Fetches a resource from the @ref World.
     * @tparam T The resource type to fetch.
     * @return A resource of type T.
     * @warning Crashes if the requested resource is not present.
     */
    template <typename T>
    auto resource() noexcept -> auto& {
        return world().resource<T>();
    }

    /**
     * @brief Adds a resources to the @ref World.
     * @tparam T The resource types to add.
     * @return A reference to the app for use in the builder pattern.
     * @note To have the function return the component, call app.world().add_resource<T>()
     */
    template <typename T, typename... Args>
    auto add_resource(Args&&... args) -> App& {
        world().add_resource<T>(std::forward<Args>(args)...);
        return *this;
    }

    /**
     * @brief Adds a list of resources to the world.
     * @note All resources added this way will simply be default constructed.
     * @tparam Resources The resource types to add.
     * @return A reference to the app for use in the builder pattern.
     */
    template <typename... Resources>
        requires(std::is_default_constructible_v<Resources> && ...)
    auto add_resources() -> App& {
        (world().add_resource<Resources>(), ...);
        return *this;
    }

    /**
     * @brief Removes all resources from the provided type list.
     * @tparam Resources The resource types to remove.
     * @return A reference to the app for use in the builder pattern.
     */
    template <typename... Resources>
    auto remove_resources() -> App& {
        (world().remove_resource<Resources>(), ...);
        return *this;
    }

    /**
     * @brief Runs a single schedule phase. Shorthand for calling app.scheduler().run_phase()
     * @param schedule_phase The phase to run.
     */
    auto run_phase(const schedule::SchedulePhase schedule_phase) -> void {
        m_scheduler.run_phase(schedule_phase, m_world);
    }

    /**
     * @brief Runs one tick of the world. Shorthand for calling app.scheduler().step().
     */
    auto step() -> void {
        m_scheduler.step(m_world);
    }

    /**
     * @brief Returns the @ref schedule::Scheduler of the application.
     * @return The @ref schedule::Scheduler of the application.
     */
    template <typename Self>
    auto scheduler(this Self&& self) noexcept -> auto&& { return std::forward<Self>(self).m_scheduler; }

private:
    ecs::World m_world;
    schedule::Scheduler m_scheduler;
    Plugins m_plugins;
    MainLoop m_loop = [] (App& app) {
        app.scheduler().run_phase(schedule::SchedulePhase::OnStart, app.world());

        auto lifetime = app.world().resource<AppLifetime>();

        while (!lifetime->should_exit) {
            app.scheduler().step(app.world());
        }

        app.scheduler().run_phase(schedule::SchedulePhase::OnEnd, app.world());
    };
};

} // namespace siren

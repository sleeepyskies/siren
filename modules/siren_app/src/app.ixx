module;

#include <entt/entt.hpp>
#include <utility>
#include <functional>
#include <ostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <ranges>
#include <any>

export module siren.app;

import siren.ecs;
import siren.reflect;
import siren.log;
import siren.common;

namespace siren {

export struct OnLoad { };

export struct First { };

export struct PreUpdate { };

export struct Update { };

export struct Render { };

export struct End { };

export struct OnShutdown { };

/** @brief Makes sure we swap all event buffers once a frame. */
auto handle_swap_events(const ecs::Resource<ecs::EventBus> event_bus) -> void {
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

    /** @brief Called once when the plugin is registered. */
    virtual auto construct(App& app) const -> void = 0;

    /** @brief Called once when the application destructs in reverse registration order. */
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
     *     while (app.world().resource<AppLifetime>().should_exit == true)
     *         app.scheduler().step(app.world());
     *     app.scheduler().run_phase(schedule::SchedulePhase::End, app.world());
     * };
     * @endcode
     */
    using MainLoop = std::function<void(App&)>;

    /** @brief Constructs a siren application. */
    App() : m_resolver(m_world), m_scheduler(m_resolver) {
        log::init();

        m_scheduler.add_phase<OnLoad>().once();

        m_scheduler.add_phase<First>().loop();
        m_scheduler.add_phase<PreUpdate>().after<First>().loop();
        m_scheduler.add_phase<Update>().after<PreUpdate>().loop();
        m_scheduler.add_phase<Render>().after<Update>().loop();

        m_scheduler.add_phase<OnShutdown>().once();

        this
              ->add_resource<AppLifetime>()
               .add_resource<ecs::SignalBus>()
               .add_resource<ecs::Signals>(*resource<ecs::SignalBus>(), m_resolver)
               .add_resource<ecs::EventBus>();

        scheduler().add_system<First>(handle_swap_events);
    }

    /**
     * @brief Destructor of the application. Shuts down all plugins in
     * reverse order of registration.
     */
    ~App() { for (auto& plugin : std::views::reverse(m_plugins)) plugin->shutdown(*this); }

    App(const App&)            = delete;
    App(App&&)                 = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&)      = delete;

    /** @brief Runs the main loop of the application. */
    auto run() -> void { m_loop(*this); }

    /** @brief Runs a single schedule phase immediately. */
    template <typename Phase>
    auto run_phase() -> void { m_scheduler.run_phase<Phase>(m_world); }

    /**
     * @brief Adds and registers a new plugin with the application.
     * @note If adding a plugin twice, any attempts after the first will
     * have no effect.
     * @tparam TPlugin The plugin to register
     * @param plugin The plugin instance to register. Will default construct
     * an instance if none is provided.
     * @return A reference to this app for the builder pattern.
     */
    template <IsPlugin TPlugin>
    auto add_plugin(TPlugin&& plugin = { }) -> App& {
        if (has_plugin<TPlugin>()) {
            log::warn("Plugin {} has already been added, cannot add a plugin twice.", TypeName<TPlugin>::value());
            return *this;
        }

        auto plugin_ptr = std::make_unique<TPlugin>(std::move(plugin_ptr));
        plugin_ptr->construct(*this);
        m_plugins.emplace_back(std::move(plugin_ptr));
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
     * @brief Fetches a resource from the @ref World.
     * @tparam T The resource type to fetch.
     * @return A resource of type T.
     * @warning Crashes if the requested resource is not present.
     */
    template <typename T>
    auto resource() noexcept -> ecs::Resource<T> {
        return world().resource<T>();
    }

    /**
     * @brief Adds a resources to the @ref World.
     * @tparam T The resource types to add.
     * @return A reference to the app for use in the builder pattern.
     * @note To have the function return the component, call app.world().add_resource<T>()
     */
    template <typename T, typename... Args>
        requires(std::is_constructible_v<T, Args...>)
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

    template <typename Phase, ecs::IsSystem System>
    auto add_system(System&& system, const std::string& name = "unnamed") -> App& {
        scheduler().add_system<Phase>(std::forward<System>(system), name);
        return *this;
    }

    /** @brief Returns the @ref ecs::World of the application. */
    auto world() noexcept -> ecs::World& { return m_world; }
    /** @brief Returns the @ref ecs::World of the application. */
    const auto world() const noexcept -> const ecs::World& { return m_world; }

    /** @brief Returns the @ref schedule::Scheduler of the application. */
    auto scheduler() noexcept -> ecs::Scheduler& { return m_scheduler; }
    /** @brief Returns the @ref schedule::Scheduler of the application. */
    const auto scheduler() const noexcept -> const ecs::Scheduler& { return m_scheduler; }

private:
    ecs::World m_world;
    ecs::Resolver m_resolver;
    ecs::Scheduler m_scheduler;
    Plugins m_plugins;
    MainLoop m_loop = [] (App& app) {
        auto& world   = app.world();
        auto lifetime = world.resource<AppLifetime>();

        app.scheduler().run_phase<OnLoad>(world);

        while (!lifetime->should_exit) {
            app.scheduler().run_phase<First>(world);
            app.scheduler().run_phase<PreUpdate>(world);
            app.scheduler().run_phase<Update>(world);
            app.scheduler().run_phase<Render>(world);
        }

        app.scheduler().run_phase<OnShutdown>(world);
    };
};

} // namespace siren

export module siren.app.resources;

namespace siren {

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

} // namespace siren


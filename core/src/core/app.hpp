#pragma once

#include "config.hpp"
#include "spch.hpp"


namespace siren::core
{

/**
 * @class App
 * @brief The main application class of siren.
 *
 * In order to create a Siren app, one should create a new
 * class deriving from this class, and override the
 * on_init(), on_update() and on_render() methods.
 *
 * In order to instantiate a new app, the helper method
 * App::create() is provided. Note that the sub app must
 * provide the @ref Config in the constructor when
 * initializing the app.
 */
class App {
public:
    App(const App&)            = delete;
    App(App&&)                 = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&)      = delete;

    /**
     * @brief Starts the main loop of siren.
     */
    auto run() -> void;

    /**
     * @brief Creates and inits a siren application.
     * @tparam TApp The application type.
     * @tparam Args The application constructor arguments.
     * @param args The specific constructor arguments.
     * @return A unique pointer to a siren application.
     */
    template <typename TApp, typename... Args>
        requires(std::derived_from<TApp, App>)
    static auto create(Args&&... args) -> std::unique_ptr<TApp> {
        auto app = std::make_unique<TApp>(std::forward<Args>(args)...);

        if (!app) {
            std::println(stderr, "Could not create a siren application. :(");
            std::abort();
        }

        app->init();
        return app;
    }

protected:
    explicit App(const Config& config);
    virtual ~App() = 0;

    /**
     * @brief Initialization hook.
     */
    virtual auto on_init() -> void = 0;

    /**
     * @brief The update hook.
     * @param delta The time in seconds since the last frame.
     */
    virtual auto on_update(float delta) -> void = 0;

    /**
     * @brief The render hook.
     */
    virtual auto on_render() -> void = 0;

private:
    /** @brief Initializes core sub systems */
    auto init() -> void;

    /** @brief Run flag. */
    bool m_running;
    /** @brief Configuration used for initializing siren. */
    Config m_config;
    /** @brief Core logger */
    logger_ptr m_log;
};

} // namespace siren::core


export module siren.app.plugin;

namespace siren {

class App;

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

} // namespace siren

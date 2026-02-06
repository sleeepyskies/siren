#pragma once

#include "utilities/spch.hpp"


namespace siren::core
{
/**
 * @brief The main Application class of siren.
 */
class App {
public:
    App(const App&)            = delete;
    App(App&&)                 = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&)      = delete;

    /// @brief Application wide properties and configurations.
    struct Description {
        /// @brief The name of the Application.
        std::string name = "Siren";

        /// @brief The RenderAPI currently active.
        enum class RenderAPI { None, OpenGL } renderAPI = RenderAPI::OpenGL;

        /// @brief The detected OS the exe is being run on. Should never be changed during runtime.
        const enum class OS { None, Windows } OS = OS::Windows;
    };

    /// @brief Returns a reference to the App singleton.
    static auto get() -> App&;
    /// @brief Starts the main loop, will run until told to stop.
    void run() const;
    /// @brief Initializes the App instance. Should register all required modules.
    virtual void init();
    /// @brief onUpdate Hook. Inheritors should provide all update logic here.
    virtual void on_update(float delta) = 0;
    /// @brief onRender Hook. Inheritors should provide all render logic here.
    virtual void on_render() = 0;

    /// @brief Creates and returns a singleton App reference.
    template <typename TApp>
        requires(std::derived_from<TApp, App>)
    static TApp& create(const Description& description) {
        SIREN_ASSERT(!s_instance, "Cannot create multiple instances of Application");
        s_instance = new TApp(description);
        SIREN_ASSERT(s_instance, "App initialization failed");
        static_cast<TApp*>(s_instance)->init();
        return *static_cast<TApp*>(s_instance);
    }

    /// @brief Switches out the current backend and resets all dependent systems.
    void switch_render_api(Description::RenderAPI api);
    /// @brief Returns the App description.
    Description description() const;

protected:
    explicit App(const Description& properties);
    virtual ~App() = 0;

    // todo: dont need this as a singleton due to Locator
    static inline App* s_instance;

private:
    Description m_description{ };
    bool m_running = true;
};
} // namespace siren::core


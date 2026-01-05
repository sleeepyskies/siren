/**
 * @file App.hpp
 * @brief The main API for Siren. Holds and provides access to Layers, Modules and holds the main
 * loop.
 */
#pragma once

#include "Module.hpp"
#include "events/EventBus.hpp"
#include "utilities/spch.hpp"


namespace siren::core
{
// todo: can we design it such that App doesn't need to be a singleton?

/**
 * @brief App is the central point of Siren. It is a singleton owning all Modules and Layers.
 */
class App
{
public:
    /// @brief Application wide properties and configurations.
    struct Properties
    {
        /// @brief The name of the Application.
        std::string name = "Siren";

        /// @brief The RenderAPI currently active. Changing this requires reinitializing some
        /// modules.
        enum class RenderAPI { None, OpenGL } renderAPI = RenderAPI::OpenGL;

        /// @brief The detected OS the exe is being run on. Should never be changed during runtime.
        const enum class OS { None, Windows } OS = OS::Windows;
    };

    /// @brief Returns a reference to the App singleton.
    static App& Get();

    /// @brief Starts the main loop, will run until told to stop.
    void Run();

    /// @brief Initializes the App instance. Should register all required modules.
    virtual void Init();

    /// @brief onUpdate Hook. Inheritors should provide all update logic here.
    virtual void OnUpdate(float delta) = 0;

    /// @brief onRender Hook. Inheritors should provide all render logic here.
    virtual void OnRender() = 0;

    /// @brief Creates and returns a singleton App reference.
    template <typename TApp>
        requires(std::derived_from<TApp, App>)
    static TApp& Create(const Properties& properties)
    {
        SirenAssert(!s_instance, "Cannot create multiple instances of Application");
        s_instance = new TApp(properties);
        SirenAssert(s_instance, "App initialization failed");
        static_cast<TApp*>(s_instance)->Init();
        return *static_cast<TApp*>(s_instance);
    }

    /// @brief Creates and registers TModule for use throughout siren.
    template <typename TModule>
        requires(std::derived_from<TModule, Module>)
    App& RegisterModule()
    {
        const std::type_index index = typeid(TModule);
        if (m_modules.contains(index)) {
            return Get();
        }

        m_modules.emplace(index, std::make_unique<TModule>());
        m_modules[index]->Init();
        return Get();
    }

    /// @brief Returns the TModule of this App
    template <typename TModule>
        requires(std::derived_from<TModule, Module>)
    TModule* GetModule()
    {
        const std::type_index index = typeid(TModule);
        if (!m_modules.contains(index)) {
            return nullptr;
        }
        return static_cast<TModule*>(m_modules[index].get());
    }

    /// @brief Switches out the current backend and resets all dependent systems.
    void SwitchRenderApi(Properties::RenderAPI renderAPI);

    /// @brief Returns the App properties.
    Properties GetProperties() const;
    /// @brief Returns the event bus.
    EventBus& GetEventBus();

protected:
    explicit App(const Properties& properties);
    virtual ~App() = 0;

    static inline App* s_instance;

private:
    App(const App&)            = delete;
    App& operator=(const App&) = delete;

    Properties m_properties{ };

    HashMap<std::type_index, Own<Module>> m_modules{ };

    EventBus m_eventBus{ };

    bool m_running = true;
};
} // namespace siren::core

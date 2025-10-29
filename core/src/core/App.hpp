/**
 * @file App.hpp
 * @brief The main API for Siren. Holds and provides access to Layers, Modules and holds the main
 * loop.
 */
#pragma once

#include "Layer.hpp"
#include "Module.hpp"
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
        std::string name = "siren";

        /// @brief The RenderAPI currently active. Changing this requires reinitializing some
        /// modules.
        enum class RenderAPI
        {
            NONE,
            OPENGL,
        } renderAPI = RenderAPI::OPENGL;

        /// @brief The detected OS the exe is being run on. Should never be changed during runtime.
        const enum class OS
        {
            NONE,
            WINDOWS
        } OS = OS::WINDOWS;
    };

    /// @brief Creates and returns a singleton App reference.
    static App& create(const Properties& properties);
    /// @brief Returns a reference to the App singleton.
    static App& get();
    virtual void init();

    /// @brief Creates a new TLayer and pushes it to the layer stack.
    template <typename TLayer>
        requires(std::derived_from<TLayer, Layer>)
    App& pushLayer()
    {
        m_layers.emplace_back(createOwn<TLayer>());
        m_layers.back()->onAttach();
        return get();
    }

    /// @brief Removes the given layer from the layer stack if it is present.
    void popLayer(Layer* layer);

    /// @brief Creates and registers TModule for use throughout siren.
    template <typename TModule>
        requires(std::derived_from<TModule, Module>)
    App& registerModule()
    {
        const std::type_index index = typeid(TModule);
        if (m_modules.contains(index)) {
            return get();
        }

        m_modules.emplace(index, std::make_unique<TModule>());
        m_modules[index]->initialize();
        return get();
    }

    /// @brief Returns the TModule of this App
    template <typename TModule>
        requires(std::derived_from<TModule, Module>)
    TModule* getModule()
    {
        const std::type_index index = typeid(TModule);
        if (!m_modules.contains(index)) {
            return nullptr;
        }
        return static_cast<TModule*>(m_modules[index].get());
    }

    /// @brief Starts the main loop, will run until told to stop.
    void run();

    /// @brief Switches out the current backend and resets all dependent systems.
    void switchRenderAPI(Properties::RenderAPI renderAPI);

    /// @brief Returns the App properties.
    Properties getProperties() const;

protected:
    explicit App(const Properties& properties);
    virtual ~App();

    static inline App* s_instance;

private:
    App(const App&)            = delete;
    App& operator=(const App&) = delete;

    Properties m_properties{ };

    HashMap<std::type_index, Own<Module>> m_modules{ };
    std::vector<Own<Layer>> m_layers{ };
};

} // namespace siren::core

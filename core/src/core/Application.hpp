#pragma once

#include "assets/AssetManager.hpp"
#include "core/Layer.hpp"
#include "core/Time.hpp"
#include "core/Window.hpp"
#include "events/Event.hpp"
#include "utilities/spch.hpp"

namespace siren::core
{

class Application
{
public:
    struct Properties {
        std::string name = "siren";
        fs::path workingDirectory;
        Window::Properties windowProperties;
    };

    explicit Application(const Properties& properties);
    ~Application();
    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    static Application& get();
    Window& getWindow() const;
    const Time& getTime() const;
    assets::AssetManager& getAssetManager() const;
    const Properties& getProperties();

    void run();
    void stop();

    template <typename L>
        requires(std::is_base_of_v<Layer, L>)
    void pushLayer()
    {
        m_layerStack.push_back(makeUref<L>());
        m_layerStack.back()->onAttach();
    }

private:
    static Application* s_instance;
    Properties m_properties;
    Ref<Window> m_window = nullptr;
    bool m_running       = true;

    std::vector<Uref<Layer>> m_layerStack{};
    std::queue<Uref<events::Event>> m_eventQueue{};

    Time m_time{};

    Uref<assets::AssetManager> m_assetManager = nullptr;

    void onEvent(const events::Event& e);
};

} // namespace siren::core

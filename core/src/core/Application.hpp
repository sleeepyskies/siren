#pragma once

#include "FileSystemManager.hpp"
#include "assets/AssetManager.hpp"
#include "core/Layer.hpp"
#include "core/Time.hpp"
#include "core/Window.hpp"
#include "event/Event.hpp"
#include "renderer/shaders/ShaderManager.hpp"
#include "utilities/spch.hpp"

#include "ui/UIRenderer.hpp"

struct CommandLineParams;
namespace siren::core
{

class Application
{
public:
    struct Properties {
        std::string name = "siren";
        Window::Properties windowProperties;
    };

    explicit Application(const Properties& properties);
    virtual ~Application();
    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    static Application& get();
    Window& getWindow() const;
    const Time& getTime() const;
    const Properties& getProperties();

    assets::AssetManager& getAssetManager() const;
    renderer::ShaderManager& getShaderManager() const;
    assets::AssetRegistry& getAssetRegistry() const;
    FileSystemManager& getFileSystemManager() const;
    ui::UIRenderer& getUIManager() const;

    void run();
    void stop();

    template <typename L>
        requires(std::derived_from<L, Layer>)
    void pushLayer()
    {
        m_layerStack.push_back(makeUref<L>());
        m_layerStack.back()->setEventCallback([this](const event::Event& e) { this->onEvent(e); });
        m_layerStack.back()->onAttach();
    }

private:
    static Application* s_instance;
    Properties m_properties;
    Ref<Window> m_window = nullptr;
    bool m_running       = true;

    std::vector<Uref<Layer>> m_layerStack{};
    std::queue<Uref<event::Event>> m_eventQueue{};

    Time m_time{};

    Uref<assets::AssetManager> m_assetManager     = nullptr;
    Uref<renderer::ShaderManager> m_shaderManager = nullptr;
    Uref<FileSystemManager> m_fileSystemManager   = nullptr;
    Uref<ui::UIRenderer> m_uiManager              = nullptr;

    void onEvent(const event::Event& e);
};

} // namespace siren::core

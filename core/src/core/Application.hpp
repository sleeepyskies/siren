#pragma once

#include "core/Layer.hpp"
#include "core/Time.hpp"
#include "core/Window.hpp"
#include "utilities/spch.hpp"

namespace core
{

class Application
{
public:
    struct Specification {
        std::string name = "siren";
        std::string workingDirectory;
        Window::Specification windowSpec;
    };

    explicit Application(const Specification& specification);
    ~Application();

    static Application& get();

    void run();
    void stop();

    Window& getWindow() const;

    template <typename L>
        requires(std::is_base_of_v<Layer, L>)
    void pushLayer()
    {
        m_layerStack.push_back(makeUref<L>());
        m_layerStack.back()->onAttach();
    }

private:
    static Application* s_application;
    Specification m_specification;

    bool m_running = true;

    ref<Window> m_window = nullptr;
    std::vector<uref<Layer>> m_layerStack{};

    Time m_time{};
};

} // namespace core

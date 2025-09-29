#pragma once

#include "core/Layer.hpp"
#include "core/Time.hpp"
#include "core/Window.hpp"
#include "events/Event.hpp"
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
    ref<Window> m_window = nullptr;
    bool m_running       = true;

    std::vector<uref<Layer>> m_layerStack{};
    std::queue<uref<Event>> m_eventQueue{};

    Time m_time{};

    void onEvent(Event& e);
};

} // namespace core

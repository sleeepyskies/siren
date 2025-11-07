#include "App.hpp"

#include "Timer.hpp"
#include "Module.hpp"
#include "input/InputModule.hpp"
#include "window/WindowModule.hpp"

#include <ranges>


namespace siren::core
{

App::~App()
{
    for (const auto& val : m_modules | std::views::values) {
        val->shutdown();
    }
    s_instance = nullptr;
}

App& App::get()
{
    SirenAssert(s_instance, "Attempting to access Application before an instance has been made");
    return *s_instance;
}

void App::run()
{
    Timer::initialize();
    bool running = true;

    // cache access to core modules
    auto* input  = getModule<InputModule>();
    auto* window = getModule<WindowModule>();

    while (running) {
        Timer::tick();
        input->update();
        window->pollEvents();

        running = !window->shouldClose();

        if (!running) {
            break;
        }

        s_instance->onUpdate(Timer::getDelta());
        s_instance->onRender();

        window->swapBuffers();
    }

    // fixme: in what order to shutdown?
    for (const auto& module : m_modules | std::views::values) {
        module->shutdown();
    }
}


void App::initialize()
{
    s_instance->registerModule<WindowModule>();
    s_instance->registerModule<InputModule>();
}

void App::switchRenderAPI(const Properties::RenderAPI renderAPI)
{
    // no work to be done :D
    if (renderAPI == m_properties.renderAPI) { return; }

    m_properties.renderAPI = renderAPI;
    // todo: reinit things like window, renderer, time
}

App::Properties App::getProperties() const
{
    return m_properties;
}

App::App(const Properties& properties) : m_properties(properties)
{
    s_instance = this;
    s_instance->initialize();
}

} // namespace siren::core

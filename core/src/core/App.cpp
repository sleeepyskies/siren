#include "App.hpp"

#include "Timer.hpp"
#include "Module.hpp"
#include "input/InputModule.hpp"
#include "window/WindowModule.hpp"

#include <ranges>

#include "reflex.hpp"

#include "ecs/components/DirectionalLightComponent.hpp"
#include "ecs/components/HierarchyComponent.hpp"
#include "ecs/components/MeshComponent.hpp"
#include "ecs/components/PointLightComponent.hpp"
#include "ecs/components/RenderContextComponent.hpp"
#include "ecs/components/TagComponent.hpp"


namespace siren::core
{

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
    // these are seen as core and siren cannot work without
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

App::~App()
{
    for (const auto& val : m_modules | std::views::values) {
        val->shutdown();
    }
    s_instance = nullptr;
}

} // namespace siren::core

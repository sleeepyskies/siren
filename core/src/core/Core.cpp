#include "Core.hpp"

#include "App.hpp"
#include "assets/AssetModule.hpp"
#include "input/InputModule.hpp"
#include "renderer/RenderModule.hpp"
#include "time/TimeModule.hpp"
#include "window/WindowModule.hpp"

namespace siren::core
{

App& app()
{
    return App::get();
}

WindowModule& window()
{
    auto* mod = app().getModule<WindowModule>();
    SirenAssert(mod, "WindowModule not found");
    return *mod;
}

InputModule& input()
{
    auto* mod = app().getModule<InputModule>();
    SirenAssert(mod, "InputModule not found");
    return *mod;
}

TimeModule& time()
{
    auto* mod = app().getModule<time::TimeModule>();
    SirenAssert(mod, "TimeModule not found");
    return *mod;
}

AssetModule& assets()
{
    auto* mod = app().getModule<assets::AssetModule>();
    SirenAssert(mod, "AssetModule not found");
    return *mod;
}

RenderModule& renderer()
{
    auto* mod = app().getModule<renderer::RenderModule>();
    SirenAssert(mod, "RenderModule not found");
    return *mod;
}

} // namespace siren::core

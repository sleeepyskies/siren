#include "Core.hpp"

#include "App.hpp"
#include "assets/AssetModule.hpp"
#include "filesystem/FileSystemModule.hpp"
#include "input/InputModule.hpp"
#include "renderer/RenderModule.hpp"
#include "window/WindowModule.hpp"


namespace siren::core
{

App& app()
{
    return App::get();
}

FileSystemModule& filesystem()
{
    auto* mod = app().getModule<FileSystemModule>();
    SirenAssert(mod, "FileSystemModule not found");
    return *mod;
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

AssetModule& Assets()
{
    auto* mod = app().getModule<AssetModule>();
    SirenAssert(mod, "AssetModule not found");
    return *mod;
}

RenderModule& Renderer()
{
    auto* mod = app().getModule<RenderModule>();
    SirenAssert(mod, "RenderModule not found");
    return *mod;
}

} // namespace siren::core

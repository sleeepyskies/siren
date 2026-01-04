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
    return App::Get();
}

FileSystemModule& filesystem()
{
    auto* mod = app().GetModule<FileSystemModule>();
    SirenAssert(mod, "FileSystemModule not found");
    return *mod;
}

WindowModule& window()
{
    auto* mod = app().GetModule<WindowModule>();
    SirenAssert(mod, "WindowModule not found");
    return *mod;
}

InputModule& input()
{
    auto* mod = app().GetModule<InputModule>();
    SirenAssert(mod, "InputModule not found");
    return *mod;
}

AssetModule& Assets()
{
    auto* mod = app().GetModule<AssetModule>();
    SirenAssert(mod, "AssetModule not found");
    return *mod;
}

RenderModule& Renderer()
{
    auto* mod = app().GetModule<RenderModule>();
    SirenAssert(mod, "RenderModule not found");
    return *mod;
}

} // namespace siren::core

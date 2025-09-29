#pragma once

#include "scene/Scene.hpp"

namespace core::renderer
{

class Renderer
{
public:
    static void init();
    static void shutdown();

    static void beginScene();
    static void endScene();

    static void submitScene(const Scene& scene);
};

} // namespace core::renderer

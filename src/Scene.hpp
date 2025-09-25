#pragma once

#include "Model.hpp" // needs to be included first since it includes glad.h

#include "Camera.hpp"
#include "Lights.hpp"
#include <types.hpp>

class Scene
{
public:
    explicit Scene(const Window& window);

    void onUpdate(float delta, const Window& window);
    void draw(const Shaders& shaders) const;
    void onWindowResize(int width, int height);

private:
    Camera m_camera;
    std::vector<uref<Light>> m_lights{};
    std::vector<uref<Model>> m_models{};
};

#pragma once

#include "Model.hpp" // needs to be included first since it includes glad.h

#include "Lights.hpp"
#include "core/Window.hpp"
#include "scene/Camera.hpp"
#include "utilities/spch.hpp"

namespace core
{

class Scene
{
public:
    explicit Scene(const Window& window);

    void onUpdate(float delta, const Window& window);
    void draw(const renderer::Shader& objectShader, const renderer::Shader& lightShader) const;
    void onWindowResize(int width, int height);

private:
    Camera m_camera;
    std::vector<uref<Model>> m_models{};
    std::vector<uref<Model>> m_lightModels{};

    // lights, not great way to do this but whatever
    std::vector<DirectionalLight> m_directionalLights{};
    std::vector<PointLight> m_pointLights{};
    std::vector<SpotLight> m_spotLights{};
    renderer::UniformBuffer m_dirLightBuffer{};
    renderer::UniformBuffer m_pointLightBuffer{};
    renderer::UniformBuffer m_spotLightBuffer{};
};

} // namespace core

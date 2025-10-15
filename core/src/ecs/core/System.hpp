#pragma once

namespace siren::ecs
{

class Scene;

/**
 * @brief The interface that all systems must implement. Allows for the data held in Component
 * objects to be changed dynamically during the runtime of the engine.
 */
class System
{
public:
    virtual ~System() = default;

    /// @brief Is called once as soon as the system becomes active
    virtual void onReady(Scene& scene){};

    /// @brief Is called once just before the scene becomes inactive
    virtual void onShutdown(Scene& scene){};

    /// @brief Called once every frame before rendering
    virtual void onUpdate(float delta, Scene& scene){};

    /// @brief Called once every frame after updating
    virtual void onRender(Scene& scene){};
};

} // namespace siren::ecs

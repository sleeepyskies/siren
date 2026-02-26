#pragma once

namespace siren::core
{

class Scene;

/**
 * @brief The interface that all systems must implement. Allows for the data held in Component
 * objects to be changed dynamically during the runtime of the engine.
 */
class System {
public:
    /// @brief Is called once as soon as the system becomes active
    virtual void onReady(Scene& scene) { };

    /// @brief Is called once just before the scene becomes inactive
    virtual void onShutdown(world& scene) { };

    /// @brief Called once every frame before rendering
    virtual void onUpdate(float delta, world& scene) { };

    /// @brief Called once every frame after updating
    virtual void on_render(world& scene) { };

    /// @brief Called each time the scene is paused.
    virtual void onPause(world& scene) { };

    /// @brief Called each time the scene is resumed.
    virtual void onResume(world& scene) { };
};

} // namespace siren::ecs

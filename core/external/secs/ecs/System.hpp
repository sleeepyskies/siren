#pragma once

namespace secs
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

    /// @brief Handles updating this systems logic.
    virtual void onUpdate(float delta, Scene& scene) = 0;
};

} // namespace secs

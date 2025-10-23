/**
 * @file Layer.hpp
 */
#pragma once

namespace siren::core
{

/**
 * @brief A Layer in Siren should be used for any user updatable logic.
 * Opposed to @ref Module, Layers in Siren do not provide utility, but instead logic. They may
 * manage their own data as well, which they can update and render in the respective function calls.
 */
class Layer
{
public:
    virtual ~Layer() = default;

    /// @brief Is called each time the Layer is pushed to the layer stack.
    virtual void onAttach() = 0;

    /// @brief Is called each time the Layer is popped from the layer stack.
    virtual void onDetach() = 0;

    /// @brief Is called one time each frame, with the time elapsed from last frame. Should contain
    /// update logic.
    virtual void onUpdate(float delta) = 0;

    /// @brief Is called one time each frame. Should contain render logic.
    virtual void onRender() = 0;

    /// @brief Is called one time each frame. Should contain UI render logic.
    virtual void onUiRender() = 0;

    /// @brief Returns this layers name. Mainly used for debugging purposes.
    virtual const char* getName() const = 0;
};

} // namespace siren::core

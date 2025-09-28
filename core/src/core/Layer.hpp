#pragma once

namespace core
{

class Layer
{
public:
    virtual ~Layer() = default;

    virtual void onAttach()            = 0;
    virtual void onDetach()            = 0;
    virtual void onUpdate(float delta) = 0;
    virtual void onRender()            = 0;
};

} // namespace core

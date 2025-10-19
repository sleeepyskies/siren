#pragma once

#include "utilities/spch.hpp"

namespace siren::editor
{

/**
 * @brief A Widget in the Siren Editor is a UI component that is fixed and is always
 * rendered.
 */
class Widget
{
public:
    virtual ~Widget()                   = default;
    virtual void onUiRender()           = 0;
    virtual std::string getName() const = 0;
};

} // namespace siren::editor
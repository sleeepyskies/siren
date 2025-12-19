#pragma once

#include <variant>
#include <limits>


namespace siren::editor
{
// imgui clamps internal max and min size to these, is range [-1,000,000 to 1,000,000]

static constexpr float DEFAULT_MIN = -1e6f;
static constexpr float DEFAULT_MAX = 1e6f;

struct SliderParams
{
    float min;
    float max;
};

struct DragParams
{
    float speed;
    float min;
    float max;
};

struct InputParams
{
    float step;
    float stepFast;
};

using InputConfig = std::variant<std::monostate, SliderParams, DragParams, InputParams>;

struct GuiMeta
{
    enum GuiFlags { NONE = 0, COLOR = 0x1 };

    static GuiMeta create() { return GuiMeta{ }; }

    static GuiMeta none() { return create(); }

    static GuiMeta color()
    {
        GuiMeta m;
        m.flags = COLOR;
        return m;
    }
    static GuiMeta slider(
        const float min = DEFAULT_MIN,
        const float max = DEFAULT_MAX
    )
    {
        GuiMeta m;
        m.inputConfig = SliderParams{ min, max };
        return m;
    }
    static GuiMeta drag(
        const float speed = 1.f,
        const float min   = DEFAULT_MIN,
        const float max   = DEFAULT_MAX
    )
    {
        GuiMeta m;
        m.inputConfig = DragParams{ speed, min, max };
        return m;
    }
    static GuiMeta input(const float step = 1.f, const float stepFast = 1.f)
    {
        GuiMeta m;
        m.inputConfig = InputParams{ step, stepFast };
        return m;
    }

    bool isColor() const { return (flags & COLOR) != 0; }

    bool isSlider() const { return std::holds_alternative<SliderParams>(inputConfig); }

    bool isDrag() const { return std::holds_alternative<DragParams>(inputConfig); }

    bool isInput() const { return std::holds_alternative<InputParams>(inputConfig); }

    SliderParams getSliderParams() const { return std::get<SliderParams>(inputConfig); }

    DragParams getDragParams() const { return std::get<DragParams>(inputConfig); }

    InputParams getInputParams() const { return std::get<InputParams>(inputConfig); }

    int flags = NONE;
    InputConfig inputConfig{ };
};
} // namespace siren::editor

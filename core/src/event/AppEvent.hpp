#pragma once

#include "core/ErrorCode.hpp"
#include "event/Event.hpp"

namespace siren::event
{

class AppErrorEvent : public Event
{
public:
    explicit AppErrorEvent(const core::ErrorCode errorCode) : m_code(errorCode)
    {
    }

    EVENT_TYPE(EventType::AppError, EventCategory::App, AppErrorEvent);

    std::string toString() const override
    {
        return std::format("AppErrorEvent (code: {})", static_cast<int>(m_code));
    }

private:
    core::ErrorCode m_code;
};

class AppPauseEvent : public Event
{
public:
    AppPauseEvent() = default;

    EVENT_TYPE(EventType::AppPause, EventCategory::App, AppPauseEvent);

    std::string toString() const override
    {
        return std::format("AppPauseEvent");
    }
};

class AppResumeEvent : public Event
{
public:
    AppResumeEvent() = default;

    EVENT_TYPE(EventType::AppResume, EventCategory::App, AppResumeEvent);

    std::string toString() const override
    {
        return std::format("AppResumeEvent");
    }
};

class LayerPauseResumeEvent : public Event
{
public:
    // HACK: should we use an enum here instead of string??? more type safety yk :D

    LayerPauseResumeEvent(const std::string& targetLayer) : m_targetLayer(targetLayer)
    {
    }

    std::string getTargetLayer() const
    {
        return m_targetLayer;
    }

private:
    std::string m_targetLayer;
};

class LayerPauseEvent : public LayerPauseResumeEvent
{
public:
    LayerPauseEvent(const std::string& targetLayer) : LayerPauseResumeEvent(targetLayer)
    {
    }

    EVENT_TYPE(EventType::LayerPause, EventCategory::App, LayerPauseEvent);

    std::string toString() const override
    {
        return std::format("LayerPauseEvent");
    }
};

class LayerResumeEvent : public LayerPauseResumeEvent
{
public:
    LayerResumeEvent(const std::string& targetLayer) : LayerPauseResumeEvent(targetLayer)
    {
    }

    EVENT_TYPE(EventType::LayerResume, EventCategory::App, LayerResumeEvent);

    std::string toString() const override
    {
        return std::format("LayerResumeEvent");
    }
};

} // namespace siren::event

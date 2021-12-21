#pragma once

#include "Log.h"

namespace PEANUT {

#define PN_EVENT_FUNCTIONS(type)                                           \
    virtual EventType GetType() const override { return EventType::type; } \
    static EventType GetStaticType() { return EventType::type; }           \
    virtual const char* ToString() const override { return #type; }

enum class EventType {
    None,
    KeyEvent,
    MouseButtonEvent,
    MouseMovedEvent,
    ScrollEvent,
    WindowCloseEvent,
    WindowResizeEvent
};

class Event {
public:
    virtual EventType GetType() const = 0;
    virtual const char* ToString() const = 0;
};

class Dispatcher {
public:
    Dispatcher(Event& event)
        : m_event(event)
    {
    }

    template <typename T, typename Func>
    void Dispatch(const Func& f)
    {
        if (m_event.GetType() == T::GetStaticType()) {
            LOG_INFO("Dispatching {0}", m_event.ToString());
            f(static_cast<const T&>(m_event));
        }
    }

private:
    Event& m_event;
};
} // namespace PEANUT
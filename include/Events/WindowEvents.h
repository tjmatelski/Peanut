#pragma once

#include "Event.h"

namespace PEANUT
{

    class WindowCloseEvent : public Event
    {
    public:
        PN_EVENT_FUNCTIONS(WindowCloseEvent)
    };

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(const int width, const int height) : m_width(width), m_height(height) {}
        PN_EVENT_FUNCTIONS(WindowResizeEvent)
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    private:
        int m_width;
        int m_height;
    };

}
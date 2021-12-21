#pragma once

#include "../MouseCodes.h"
#include "Event.h"

namespace PEANUT {

class ScrollEvent : public Event {
public:
    ScrollEvent(double xoffset, double yoffset)
        : m_xoffset(xoffset)
        , m_yoffset(yoffset)
    {
    }
    PN_EVENT_FUNCTIONS(ScrollEvent);
    inline double GetVerticalScroll() const { return m_yoffset; }
    inline double GetHorizontalScroll() const { return m_xoffset; }

private:
    double m_xoffset;
    double m_yoffset;
};

class MouseButtonEvent : public Event {
public:
    MouseButtonEvent(MouseCode button, bool pressed)
        : m_button(button)
        , m_pressed(pressed)
    {
    }
    PN_EVENT_FUNCTIONS(MouseButtonEvent);
    inline MouseCode GetButton() const { return m_button; }
    inline bool Pressed() const { return m_pressed; }
    inline bool Released() const { return !Pressed(); }

private:
    MouseCode m_button;
    bool m_pressed;
};

class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(float x, float y)
        : m_horizontalPos(x)
        , m_verticalPos(y)
    {
    }
    PN_EVENT_FUNCTIONS(MouseMovedEvent);
    inline float HorizontalPosition() const { return m_horizontalPos; }
    inline float VerticalPosition() const { return m_verticalPos; }

private:
    float m_horizontalPos, m_verticalPos;
};

}
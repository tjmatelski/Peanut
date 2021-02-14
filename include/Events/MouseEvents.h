#pragma once

#include "Event.h"

namespace PEANUT
{

class ScrollEvent : public Event
{
public:
    ScrollEvent(double xoffset, double yoffset): m_xoffset(xoffset), m_yoffset(yoffset) {}
    PN_EVENT_FUNCTIONS(ScrollEvent);
    inline double GetVerticalScroll() { return m_yoffset; }
    inline double GetHorizontalScroll() { return m_xoffset; }

private:
    double m_xoffset;
    double m_yoffset;
};

}
#pragma once

#include "../KeyCodes.h"
#include "Event.h"

namespace PEANUT {

class KeyEvent : public Event {
public:
    KeyEvent(const KeyCode key)
        : m_key(key)
    {
    }
    PN_EVENT_FUNCTIONS(KeyEvent)
    KeyCode GetCode() const { return m_key; }

private:
    KeyCode m_key;
};

}
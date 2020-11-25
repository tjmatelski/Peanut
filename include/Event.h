#pragma once

namespace PEANUT
{

#define PN_EVENT_FUNCTIONS(type)                                           \
    virtual EventType GetType() const override { return EventType::type; } \
    static EventType GetStaticType() { return EventType::type; }

    enum class EventType
    {
        None,
        KeyEvent,
        WindowCloseEvent
    };

    class Event
    {
    public:
        virtual EventType GetType() const = 0;
    };

    class KeyEvent : public Event
    {
    public:
        KeyEvent(int code) : m_code(code) {}
        PN_EVENT_FUNCTIONS(KeyEvent)
        int GetCode() const { return m_code; }

    private:
        int m_code;
    };

    class WindowCloseEvent : public Event
    {
    public:
        PN_EVENT_FUNCTIONS(WindowCloseEvent)
    };

    class Dispatcher
    {
    public:
        Dispatcher(Event &event) : m_event(event) {}

        template <typename T, typename Func>
        void Dispatch(const Func &f)
        {
            if (m_event.GetType() == T::GetStaticType())
            {
                f(static_cast<T &>(m_event));
            }
        }

    private:
        Event &m_event;
    };
} // namespace PEANUT
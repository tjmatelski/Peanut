#pragma once

#include "Event.h"
#include <functional>

namespace PEANUT
{

class Window
{
public:
    Window(const char* title, const int width, const int height);
    ~Window();

    bool WindowShouldClose() const;
    void SwapBuffers();
    void PollEvents();

    using EventCallbackFn = std::function<void(Event& e)>;
    void SetEventCallback(const EventCallbackFn& func) { m_eventCallback = func; }

private:
    void* m_window;
    EventCallbackFn m_eventCallback;
};

}
#pragma once

#include "Events/Event.h"
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
    inline int GetWidth() const { return m_width; }
    inline int GetHeight() const { return m_height; }
    inline void* GetRawWindow() const { return m_window; }
    float GetTime() const;

    using EventCallbackFn = std::function<void(Event& e)>;
    void SetEventCallback(const EventCallbackFn& func) { m_eventCallback = func; }

private:
    void* m_window;
    int m_width;
    int m_height;
    EventCallbackFn m_eventCallback;

    void InitWindowCallbacks();
};

}
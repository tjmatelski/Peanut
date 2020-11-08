#pragma once

class Window
{
public:
    Window(const char* title, const int width, const int height);
    ~Window();

    bool WindowShouldClose() const;
    void ProcessInput();
    void SwapBuffers();
    void PollEvents();

private:
    void* m_window;
};
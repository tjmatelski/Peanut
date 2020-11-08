#include <iostream>
#include <glad/glad.h>
#include <Log.h>
#include <Window.h>

int main()
{
    Window window("My App", 800, 600);

    while (!window.WindowShouldClose())
    {
        window.ProcessInput();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.SwapBuffers();
        window.PollEvents();
    }

    LOG_INFO("Peanut!!!");
    return 0;
}
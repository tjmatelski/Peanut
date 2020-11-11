#include <iostream>
#include <glad/glad.h>
#include <Peanut.h>

// Temporary
#include "../src/GLDebug.h"

int main()
{
    Window window("My App", 800, 600);

    float vertices[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // Vertex Array Object
    VertexArray vao;

    // Vertex Buffer
    VertexBuffer vbo(sizeof(vertices), vertices);

    // Vertex Attributes
    BufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);

    vao.AddBuffer(vbo, layout);

    // Element Buffer
    IndexBuffer ebo(6, indices);

    // Shader Abstraction
    Shader shader("./res/shaders/posAndColor.shader");

    while (!window.WindowShouldClose())
    {
        window.ProcessInput();

        Renderer::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Render Triangle
        Renderer::Draw(vao, ebo, shader);

        window.SwapBuffers();
        window.PollEvents();
    }

    LOG_INFO("Peanut!!!");
    return 0;
}
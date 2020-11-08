#include <iostream>
#include <glad/glad.h>
#include <Peanut.h>

int main()
{
    Window window("My App", 800, 600);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    // Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex Buffer
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Shader Abstraction
    Shader shader("./res/shaders/basic.shader");
    shader.Use();

    while (!window.WindowShouldClose())
    {
        window.ProcessInput();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render Triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.SwapBuffers();
        window.PollEvents();
    }

    LOG_INFO("Peanut!!!");
    return 0;
}
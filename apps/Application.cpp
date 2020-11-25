#include <glad/glad.h>
#include <Peanut.h>

#include <iostream>
#include <memory>

// Temporary
#include "../src/OpenGLRenderer/GLDebug.h"

class MyApp : public PEANUT::Application
{
public:
    MyApp() : Application()
    {
        float vertices[] = {
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f   // top left
        };
        unsigned int indices[] = {
            // note that we start from 0!
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        // Vertex Array Object
        vao = std::make_unique<PEANUT::VertexArray>();

        // Vertex Buffer
        PEANUT::VertexBuffer vbo(sizeof(vertices), vertices);

        // Vertex Attributes
        PEANUT::BufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);

        vao->AddBuffer(vbo, layout);

        // Element Buffer
        ebo = std::make_unique<PEANUT::IndexBuffer>(6, indices);

        // Shader Abstraction
        shader = std::make_unique<PEANUT::Shader>("./res/shaders/twoTextureWithTransform.shader");
        shader->Use();
        shader->SetUniform1i("texture1", 0);
        shader->SetUniform1i("texture2", 1);

        // Texture
        texture = std::make_unique<PEANUT::Texture>("./res/textures/container.jpg");
        awesomeFaceTexture = std::make_unique<PEANUT::Texture>("./res/textures/awesomeface.png");
    }

    virtual void OnAttach() override
    {
    }

    virtual void OnUpdate() override
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.5f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader->SetUniformMat4("transform", transform);
        
        PEANUT::Renderer::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Render Triangle
        PEANUT::Renderer::Draw(*vao, *ebo, *shader, {texture.get(), awesomeFaceTexture.get()});
    }

    virtual void OnRemove() override
    {
        LOG_INFO("PEANUT!!!");
    }

private:
    std::unique_ptr<PEANUT::VertexArray> vao;
    std::unique_ptr<PEANUT::IndexBuffer> ebo;
    std::unique_ptr<PEANUT::Shader> shader;
    std::unique_ptr<PEANUT::Texture> texture;
    std::unique_ptr<PEANUT::Texture> awesomeFaceTexture;
};

PEANUT::Application* PEANUT::GetApplication()
{
    return new MyApp();
}
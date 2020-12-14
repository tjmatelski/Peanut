#include <glad/glad.h>
#include <Peanut.h>

#include <iostream>
#include <memory>

// Temporary
#include "../src/OpenGLRenderer/GLDebug.h"

class MyApp : public PEANUT::Application
{
public:
    MyApp() : Application(), m_clearColor(0.2f, 0.3f, 0.3f, 1.0f), m_translate(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f)
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
        shader = std::make_unique<PEANUT::Shader>("./res/shaders/twoTextureMVP.shader");
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
        // Model matrix
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, m_translate);
        transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, m_scale);
        shader->SetUniformMat4("model", transform);

        // View matrix
        glm::mat4 view(1.0f);
        view = glm::translate(view, glm::vec3(0.2f, 0.3f, 0.0f));
        shader->SetUniformMat4("view", view);

        // Projection Matrix
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        shader->SetUniformMat4("projection", projection);

        PEANUT::Renderer::ClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

        // Render Triangle
        PEANUT::Renderer::Draw(*vao, *ebo, *shader, {texture.get(), awesomeFaceTexture.get()});
    }

    virtual void OnRemove() override
    {
        LOG_INFO("PEANUT!!!");
    }

    virtual void OnImGuiUpdate() override
    {
        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.
        ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));
        ImGui::SliderFloat3("Translation", glm::value_ptr(m_translate), -1.0f, 1.0f);
        ImGui::SliderFloat3("Rotation", glm::value_ptr(m_rotation), 0.0f, 360.0f);
        ImGui::SliderFloat3("Scale", glm::value_ptr(m_scale), 0.0f, 2.0f);
        ImGui::End();
    }

private:
    std::unique_ptr<PEANUT::VertexArray> vao;
    std::unique_ptr<PEANUT::IndexBuffer> ebo;
    std::unique_ptr<PEANUT::Shader> shader;
    std::unique_ptr<PEANUT::Texture> texture;
    std::unique_ptr<PEANUT::Texture> awesomeFaceTexture;
    glm::vec4 m_clearColor;
    glm::vec3 m_translate;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
};

PEANUT::Application *PEANUT::GetApplication()
{
    return new MyApp();
}
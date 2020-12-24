#include <glad/glad.h>
#include <Peanut.h>

#include "SceneHierarchyPanel.h"

#include <iostream>
#include <memory>

// Temporary
#include "../src/OpenGLRenderer/GLDebug.h"

namespace PEANUT
{

class MyApp : public Application
{
public:
    MyApp() : Application(), m_clearColor(0.2f, 0.3f, 0.3f, 1.0f), m_translate(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f), m_orthoCamera(-static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()),
            static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()), -1.0, 1.0), m_scene(std::make_shared<Scene>()),
        m_scenePanel(m_scene)
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
        vao = std::make_unique<VertexArray>();

        // Vertex Buffer
        VertexBuffer vbo(sizeof(vertices), vertices);

        // Vertex Attributes
        BufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);

        vao->AddBuffer(vbo, layout);

        // Element Buffer
        ebo = std::make_unique<IndexBuffer>(6, indices);

        // Shader Abstraction
        shader = std::make_unique<Shader>("./res/shaders/twoTextureMVP.shader");
        shader->Use();
        shader->SetUniform1i("texture1", 0);
        shader->SetUniform1i("texture2", 1);

        // Texture
        texture = std::make_unique<Texture>("./res/textures/container.jpg");
        awesomeFaceTexture = std::make_unique<Texture>("./res/textures/awesomeface.png");

        m_scene->CreateEntity("MyEntity");
        m_scene->CreateEntity("Entity 2");
        m_scene->CreateEntity("Another Entity");
    }

    virtual void OnAttach() override
    {
    }

    virtual void OnUpdate(TimeStep timeStep) override
    {
        // Temporary camera controller
        float camSpeed = 1.0f * timeStep;
        if (Input::IsKeyPressed(KeyCode::W))
        {
            m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x, m_orthoCamera.GetPosition().y + camSpeed);
        }
        if (Input::IsKeyPressed(KeyCode::A))
        {
            m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x - camSpeed, m_orthoCamera.GetPosition().y);
        }
        if (Input::IsKeyPressed(KeyCode::S))
        {
            m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x, m_orthoCamera.GetPosition().y - camSpeed);
        }
        if (Input::IsKeyPressed(KeyCode::D))
        {
            m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x + camSpeed, m_orthoCamera.GetPosition().y);
        }

        // Model matrix
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, m_translate);
        transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, m_scale);

        shader->SetUniformMat4("model", transform);
        shader->SetUniformMat4("view", m_orthoCamera.GetViewMatrix());
        shader->SetUniformMat4("projection", m_orthoCamera.GetProjectionMatrix());

        Renderer::ClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

        // Render Triangle
        Renderer::Draw(*vao, *ebo, *shader, {texture.get(), awesomeFaceTexture.get()});
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

        ImGui::Separator();

        m_scenePanel.UpdateGui();

        ImGui::End();
    }

    virtual void OnEvent(Event& event)
    {
        Dispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent &e) {
            float aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
            m_orthoCamera.SetProjection(-(aspectRatio * 2.0f) / 2.0f, (aspectRatio * 2.0f) / 2.0f, -1.0f, 1.0f);
        });
    }

private:
    std::unique_ptr<VertexArray> vao;
    std::unique_ptr<IndexBuffer> ebo;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> texture;
    std::unique_ptr<Texture> awesomeFaceTexture;
    glm::vec4 m_clearColor;
    glm::vec3 m_translate;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    OrthoCamera m_orthoCamera;
    std::shared_ptr<Scene> m_scene;
    SceneHierarchyPanel m_scenePanel;
};

Application *GetApplication()
{
    return new MyApp();
}

}
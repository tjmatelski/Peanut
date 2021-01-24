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
    MyApp() : Application(), m_clearColor(0.2f, 0.3f, 0.3f, 1.0f), m_orthoCamera(-static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()),
            static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()), -1.0, 1.0),
        m_scene(std::make_shared<Scene>()),
        m_scenePanel(m_scene),
        m_textureLib()
    {
        m_entity = m_scene->CreateEntity("MyEntity");
        Entity ent = m_scene->CreateEntity("Entity 2");
        ent.Add<SpriteRenderComponent>(0.5f, 1.0f, 0.3f);
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

        Renderer::ClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

        Renderer2D::BeginScene(m_orthoCamera);
        m_scene->ForEachEntity([&](Entity ent){
            if (ent.Has<SpriteRenderComponent>())
            {
                const auto& spriteRender = ent.Get<SpriteRenderComponent>();
                Renderer2D::DrawQuad(ent.Get<TransformComponent>(), spriteRender.color, *m_textureLib.Load(spriteRender.texture));
            }
        });
    }

    virtual void OnRemove() override
    {
        LOG_INFO("PEANUT!!!");
    }

    virtual void OnImGuiUpdate() override
    {
        ImGui::Begin("Peanut Editor", nullptr, ImGuiWindowFlags_MenuBar); // Create a window called "Hello, world!" and append into it.
        ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor));

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
    glm::vec4 m_clearColor;
    OrthoCamera m_orthoCamera;
    std::shared_ptr<Scene> m_scene;
    SceneHierarchyPanel m_scenePanel;
    Entity m_entity;
    TextureLibrary m_textureLib;
};

Application *GetApplication()
{
    return new MyApp();
}

}
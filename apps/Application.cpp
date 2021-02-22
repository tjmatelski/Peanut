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

        dispatcher.Dispatch<ScrollEvent>([&](ScrollEvent& e) {
            m_orthoCamera.ZoomBy(e.GetVerticalScroll() / 100.0);
        });

        dispatcher.Dispatch<MouseButtonEvent>([&](MouseButtonEvent& event) {
            m_leftMousePressed = event.GetButton() == MouseCode::MOUSE_BUTTON_LEFT && event.Pressed();
        });

        dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& event) {
            glm::vec2 newPos(event.HorizontalPosition(), event.VerticalPosition());
            if (m_leftMousePressed)
            {
                glm::vec2 diff = newPos - m_mousePosition;
                diff /= 0.5 * GetWindow().GetHeight(); // Scales from 0 to pixelWidth to -1.0 to 1.0
                m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x - diff.x, m_orthoCamera.GetPosition().y + diff.y);
            }
            m_mousePosition = newPos;
        });
    }

private:
    glm::vec4 m_clearColor;
    OrthoCamera m_orthoCamera;
    std::shared_ptr<Scene> m_scene;
    SceneHierarchyPanel m_scenePanel;
    Entity m_entity;
    TextureLibrary m_textureLib;
    bool m_leftMousePressed = false;
    glm::vec2 m_mousePosition;
};

Application *GetApplication()
{
    return new MyApp();
}

}
#include <Peanut.h>
#include <glad/glad.h>

#include "SceneHierarchyPanel.h"
#include "ViewportPanel.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <utility>

// Temporary
#include "../src/OpenGLRenderer/GLDebug.h"

namespace PEANUT {

void UpdatePropertiesPanel(Entity selectedEntity);

class MyApp : public Application {
public:
    MyApp()
        : Application()
        , m_orthoCamera(-static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()),
              static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()), -1.0, 1.0)
        , m_scenePanel(m_scene)
        , m_mousePosition(0.0f, 0.0f)
        , m_frameBuffer()
        , m_viewportPanel()
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(GetWindow().GetRawWindow()), true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

        Renderer::EnableDepthTest();
    }

    void OnAttach() override
    {
    }

    void OnPreUpdate() override
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void OnUpdate(TimeStep timeStep [[maybe_unused]]) override
    {
        OnImGuiUpdate();
        AdjustRenderViewport(m_viewportPanel.GetWidth(), m_viewportPanel.GetHeight());

        m_frameBuffer.Bind();

        Renderer::ClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        Renderer::ClearBuffers();

        Renderer2D::BeginScene(m_orthoCamera);
        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<SpriteRenderComponent>()) {
                const auto& spriteRender = ent.Get<SpriteRenderComponent>();
                Renderer2D::DrawQuad(ent.Get<TransformComponent>(), spriteRender.color, TextureLibrary::Load(spriteRender.texture));
            }
        });

        m_frameBuffer.Unbind();
    }

    void OnPostUpdate() override
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void OnRemove() override
    {
        LOG_INFO("Closing Application");

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void OnImGuiUpdate()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoNavFocus;

        auto windowViewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowSize(windowViewport->Size);
        ImGui::SetNextWindowPos(windowViewport->Pos);
        ImGui::SetNextWindowViewport(windowViewport->ID);
        ImGui::Begin("Peanut Editor", nullptr, windowFlags);

        if (ImGui::Button("Run")) {
            m_runtime = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            m_runtime = false;
        }
        ImGui::SameLine();
        (m_runtime) ? ImGui::Text("Running") : ImGui::Text("Stopped");

        ImGui::Separator();

        ImGui::DockSpace(ImGui::GetID("MyDockspace"));

        UpdateMenuBar();
        m_scenePanel.UpdateGui();
        UpdatePropertiesPanel(m_scenePanel.GetSelectedEntity());
        m_viewportPanel.Update(m_frameBuffer);

        ImGui::End();
    }

    void OnEvent(Event& event) override
    {
        Dispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([&](const WindowResizeEvent& e) { OnWindowResize(e); });
        if (m_viewportPanel.IsHovered()) {
            dispatcher.Dispatch<ScrollEvent>([&](const ScrollEvent& e) { OnScroll(e); });
            dispatcher.Dispatch<MouseButtonEvent>([&](const MouseButtonEvent& e) { OnMouseButton(e); });
            dispatcher.Dispatch<MouseMovedEvent>([&](const MouseMovedEvent& e) { OnMouseMove(e); });
        }
        dispatcher.Dispatch<KeyEvent>([&](const KeyEvent& e) { OnKey(e); });
    }

private:
    void AdjustRenderViewport(float width, float height)
    {
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        m_orthoCamera.SetProjection(-(aspectRatio * 2.0f) / 2.0f, (aspectRatio * 2.0f) / 2.0f, -1.0f, 1.0f);
    }

    void OnWindowResize(const WindowResizeEvent& e)
    {
        Renderer::SetViewport(e.GetWidth(), e.GetHeight());
    }

    void OnScroll(const ScrollEvent& e)
    {
        m_orthoCamera.ZoomBy(static_cast<float>(e.GetVerticalScroll() / 100.0));
    }

    void OnMouseButton(const MouseButtonEvent& event)
    {
        m_leftMousePressed = event.GetButton() == MouseCode::MOUSE_BUTTON_LEFT && event.Pressed();
    }

    void OnMouseMove(const MouseMovedEvent& event)
    {
        glm::vec2 newPos(event.HorizontalPosition(), event.VerticalPosition());
        if (m_leftMousePressed) {
            glm::vec2 diff = newPos - m_mousePosition;
            diff /= 0.5 * GetWindow().GetHeight(); // Scales from 0 to pixelWidth to -1.0 to 1.0
            m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x - diff.x, m_orthoCamera.GetPosition().y + diff.y);
        }
        m_mousePosition = newPos;
    }

    void OnKey(const KeyEvent& event)
    {
        switch (event.GetCode()) {
        case KeyCode::ESCAPE:
            Terminate();
            break;

        default:
            break;
        }
    }

    void UpdateMenuBar()
    {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save")) {
                    auto saveFile = CreateFileSelectorDialog()->SaveFile().value_or("");
                    if (!saveFile.empty()) {
                        LOG_INFO("Saving Scene to '{0}'", saveFile);
                        SceneSerializer::Serialize(*m_scene, saveFile);
                    } else {
                        LOG_WARN("Failed to select save file. Not saving scene.");
                    }
                }
                if (ImGui::MenuItem("Open")) {
                    std::string sceneFile = CreateFileSelectorDialog()->OpenFile().value_or("");
                    if (sceneFile.find(".peanut") != std::string::npos) {
                        LOG_INFO("Opening Scene: {}", sceneFile);
                        SceneSerializer::Deserialize(sceneFile, *m_scene);
                    } else {
                        LOG_ERROR("Invalid scene file: {}", sceneFile);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    OrthoCamera m_orthoCamera;
    SceneHierarchyPanel m_scenePanel;
    bool m_leftMousePressed = false;
    glm::vec2 m_mousePosition;
    FrameBuffer m_frameBuffer;
    ViewportPanel m_viewportPanel;
};

Application* GetApplication()
{
    return new MyApp();
}

}
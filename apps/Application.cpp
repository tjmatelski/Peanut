#include "Engine.hpp"
#include "Scene/Component.h"
#include "SceneHierarchyPanel.h"
#include "ViewportPanel.h"
#include <Application.h>
#include <Events/Event.h>
#include <Events/KeyEvent.h>
#include <Events/MouseEvents.h>
#include <Events/WindowEvents.h>
#include <Input/FileSelectorDialog.h>
#include <Input/Input.h>
#include <Input/KeyCodes.h>
#include <Input/MouseCodes.h>
#include <Renderer/FrameBuffer.h>
#include <Renderer/OrthoCamera.h>
#include <Renderer/PerspectiveCamera.h>
#include <Renderer/Renderer.h>
#include <Scene/SceneSerializer.h>
#include <Utils/Log.h>
#include <Utils/Math.h>
#include <Window.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <memory>
#include <optional>
#include <string>

namespace PEANUT {
class Entity;
}
struct GLFWwindow;

namespace PEANUT {

void UpdatePropertiesPanel(Entity selectedEntity);

class MyApp : public Application {
public:
    void OnAttach() override
    {
        m_scenePanel = std::make_unique<SceneHierarchyPanel>(m_engine->GetScene());
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(m_engine->GetWindow().GetRawWindow()), true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

        m_frameBuffer.Resize(m_engine->GetWindow().GetWidth(), m_engine->GetWindow().GetHeight());
    }

    void OnPreUpdate() override
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_frameBuffer.Bind();
        OnImGuiUpdate();
        AdjustRenderViewport(m_viewportPanel.GetWidth(), m_viewportPanel.GetHeight());
    }

    void OnUpdate(double timeStep) override
    {
        if (m_viewportPanel.IsFocused()) {
            UpdateCameraPosition(timeStep);
        }
    }

    void OnPostUpdate() override
    {
        m_frameBuffer.Unbind();
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
            m_engine->StartRuntime();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            m_engine->StopRunTime();
        }
        ImGui::SameLine();
        (m_engine->IsRuntime()) ? ImGui::Text("Running") : ImGui::Text("Stopped");

        ImGui::Separator();

        ImGui::DockSpace(ImGui::GetID("MyDockspace"));

        UpdateMenuBar();
        m_scenePanel->UpdateGui();
        UpdatePropertiesPanel(m_scenePanel->GetSelectedEntity());
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
        m_engine->GetCamera().SetAspectRatio(width, height);
    }

    void OnWindowResize(const WindowResizeEvent& e)
    {
        m_frameBuffer.Resize(e.GetWidth(), e.GetHeight());
        Renderer::SetViewport(e.GetWidth(), e.GetHeight());
    }

    void OnScroll(const ScrollEvent& e)
    {
        m_orthoCamera.ZoomBy(static_cast<float>(e.GetVerticalScroll() / 100.0));
    }

    void OnMouseButton(const MouseButtonEvent& event)
    {
        m_leftMousePressed = event.GetButton() == MouseCode::MOUSE_BUTTON_LEFT && event.Pressed();
        m_rightMousePressed = event.GetButton() == MouseCode::MOUSE_BUTTON_RIGHT && event.Pressed();
    }

    void OnMouseMove(const MouseMovedEvent& event)
    {
        glm::vec2 newPos(event.HorizontalPosition(), event.VerticalPosition());
        glm::vec2 diff = newPos - m_mousePosition;
        diff /= 0.5 * m_engine->GetWindow().GetHeight(); // Scales from 0 to pixelWidth to -1.0 to 1.0
        if (m_leftMousePressed) {
            m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x - diff.x, m_orthoCamera.GetPosition().y + diff.y);
        }
        if (m_rightMousePressed) {
            constexpr float rotateScale = 25.0;
            m_engine->GetCamera().PitchBy(-diff.y * rotateScale);
            m_engine->GetCamera().YawBy(diff.x * rotateScale);
        }
        m_mousePosition = newPos;
    }

    void OnKey(const KeyEvent& event)
    {
        switch (event.GetCode()) {
        case KeyCode::ESCAPE:
            m_engine->Terminate();
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
                        SceneSerializer::Serialize(*m_engine->GetScene(), saveFile);
                    } else {
                        LOG_WARN("Failed to select save file. Not saving scene.");
                    }
                }
                if (ImGui::MenuItem("Open")) {
                    std::string sceneFile = CreateFileSelectorDialog()->OpenFile().value_or("");
                    if (sceneFile.find(".peanut") != std::string::npos) {
                        LOG_INFO("Opening Scene: {}", sceneFile);
                        SceneSerializer::Deserialize(sceneFile, *m_engine->GetScene());
                        m_engine->GetScene()->ForEachEntity([](Entity ent) {
                            if (ent.Has<PythonScriptComponent>()) {
                                LoadPythonScriptObj(ent);
                            }
                        });
                    } else {
                        LOG_ERROR("Invalid scene file: {}", sceneFile);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void UpdateCameraPosition(double dt)
    {
        double ammount = Input::IsKeyPressed(KeyCode::LEFT_SHIFT) ? 5 * dt : dt;
        if (Input::IsKeyPressed(KeyCode::W)) {
            m_engine->GetCamera().MoveForward(ammount);
        }
        if (Input::IsKeyPressed(KeyCode::A)) {
            m_engine->GetCamera().MoveLeft(ammount);
        }
        if (Input::IsKeyPressed(KeyCode::S)) {
            m_engine->GetCamera().MoveBackward(ammount);
        }
        if (Input::IsKeyPressed(KeyCode::D)) {
            m_engine->GetCamera().MoveRight(ammount);
        }
        if (Input::IsKeyPressed(KeyCode::Q)) {
            m_engine->GetCamera().MoveUp(ammount);
        }
        if (Input::IsKeyPressed(KeyCode::E)) {
            m_engine->GetCamera().MoveDown(ammount);
        }
    }

    OrthoCamera m_orthoCamera;
    std::unique_ptr<SceneHierarchyPanel> m_scenePanel;
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    glm::vec2 m_mousePosition = { 0.0, 0.0 };
    FrameBuffer m_frameBuffer = { { 100, 100 } };
    ViewportPanel m_viewportPanel;
};

Application* GetApplication()
{
    return new MyApp();
}

}
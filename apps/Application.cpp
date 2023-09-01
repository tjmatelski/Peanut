#include "SceneHierarchyPanel.h"
#include "ViewportPanel.h"
#include <Application.h> // FIXME
#include <Events/MouseEvents.h>
#include <Input/FileSelectorDialog.h>
#include <Input/Input.h>
#include <Input/KeyCodes.h>
#include <Renderer/Lights.h>
#include <Renderer/Material.h>
#include <Renderer/Model.h>
#include <Renderer/ModelLibrary.h>
#include <Renderer/PerspectiveCamera.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/Shader.h>
#include <Renderer/Texture.h>
#include <Renderer/TextureLibrary.h>
#include <Scene/Component.h>
#include <Scene/SceneSerializer.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace PEANUT {

void UpdatePropertiesPanel(Entity selectedEntity);

class MyApp : public Application {
public:
    MyApp()
        : Application()
        , m_orthoCamera(-static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()),
              static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight()), -1.0, 1.0)
        , m_perspectiveCam({ 0.0, 0.0, 0.0 })
        , m_scenePanel(m_scene)
        , m_mousePosition(0.0f, 0.0f)
        , m_frameBuffer({ GetWindow().GetWidth(), GetWindow().GetHeight() })
        , m_viewportPanel()
        , m_lightingShader("./res/shaders/Lighting.shader")
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

    void OnUpdate(TimeStep timeStep) override
    {
        UpdateCameraPosition(timeStep);
        OnImGuiUpdate();
        AdjustRenderViewport(m_viewportPanel.GetWidth(), m_viewportPanel.GetHeight());

        m_frameBuffer.Bind();

        Renderer::ClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        Renderer::ClearBuffers();

        Renderer2D::BeginScene(m_orthoCamera);

        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<SkyboxComponent>()) {
                m_skyboxShader.SetUniformMat4("view", glm::mat4(glm::mat3(m_perspectiveCam.GetViewMatrix())));
                m_skyboxShader.SetUniformMat4("projection", m_perspectiveCam.GetProjectionMatrix());
                const auto& skybox = ent.Get<SkyboxComponent>();
                Renderer::DisableDepthMask();
                Renderer::Draw(Renderer::GetSkyboxMesh(), Material({ TextureLibrary::Load(skybox.directory, Texture::Type::CubeMap) }), m_skyboxShader);
                Renderer::EnableDepthMask();
            }
        });

        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<SpriteRenderComponent>()) {
                const auto& spriteRender = ent.Get<SpriteRenderComponent>();
                Renderer2D::DrawQuad(ent.Get<TransformComponent>(), spriteRender.color, TextureLibrary::Load(spriteRender.texture));
            }
        });

        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<DirectionalLightComponent>()) {
                auto& comp = ent.Get<DirectionalLightComponent>();
                Renderer::SetDirectionalLight(
                    { comp.direction,
                        { comp.ambient, comp.ambient, comp.ambient },
                        { comp.diffuse, comp.diffuse, comp.diffuse },
                        { comp.specular, comp.specular, comp.specular } },
                    m_lightingShader);
            }
        });

        std::vector<PointLight> pointLights;
        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<PointLightComponent>()) {
                auto& comp = ent.Get<PointLightComponent>();
                PointLight pl;
                pl.active = true;
                pl.position = ent.Get<TransformComponent>().translation;
                pl.ambient = comp.ambient * comp.color;
                pl.diffuse = comp.diffuse * comp.color;
                pl.specular = comp.specular * comp.color;
                pl.constant = comp.constant;
                pl.linear = comp.linear;
                pl.quadratic = comp.quadratic;
                pointLights.push_back(pl);
            }
        });
        Renderer::SetPointLights(pointLights, m_lightingShader);

        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<ModelFileComponent>()) {
                m_lightingShader.SetUniformMat4("view", m_perspectiveCam.GetViewMatrix());
                m_lightingShader.SetUniformMat4("projection", m_perspectiveCam.GetProjectionMatrix());
                m_lightingShader.SetUniformVec3("viewPos", m_perspectiveCam.GetPosition());
                m_lightingShader.SetUniformMat4("model", ent.Get<TransformComponent>());
                Renderer::Draw(ModelLibrary::Get(ent.Get<ModelFileComponent>().file), m_lightingShader);
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
        m_perspectiveCam.SetAspectRatio(width, height);
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
        diff /= 0.5 * GetWindow().GetHeight(); // Scales from 0 to pixelWidth to -1.0 to 1.0
        if (m_leftMousePressed) {
            m_orthoCamera.SetPosition(m_orthoCamera.GetPosition().x - diff.x, m_orthoCamera.GetPosition().y + diff.y);
        }
        if (m_rightMousePressed) {
            constexpr float rotateScale = 25.0;
            m_perspectiveCam.PitchBy(-diff.y * rotateScale);
            m_perspectiveCam.YawBy(diff.x * rotateScale);
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

    void UpdateCameraPosition(double dt)
    {
        if (Input::IsKeyPressed(KeyCode::W)) {
            m_perspectiveCam.MoveForward(dt);
        }
        if (Input::IsKeyPressed(KeyCode::A)) {
            m_perspectiveCam.MoveLeft(dt);
        }
        if (Input::IsKeyPressed(KeyCode::S)) {
            m_perspectiveCam.MoveBackward(dt);
        }
        if (Input::IsKeyPressed(KeyCode::D)) {
            m_perspectiveCam.MoveRight(dt);
        }
        if (Input::IsKeyPressed(KeyCode::LEFT_SHIFT)) {
            m_perspectiveCam.MoveUp(dt);
        }
        if (Input::IsKeyPressed(KeyCode::LEFT_CONTROL)) {
            m_perspectiveCam.MoveDown(dt);
        }
    }

    OrthoCamera m_orthoCamera;
    PerspectiveCamera m_perspectiveCam;
    SceneHierarchyPanel m_scenePanel;
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    glm::vec2 m_mousePosition;
    FrameBuffer m_frameBuffer;
    ViewportPanel m_viewportPanel;
    Shader m_lightingShader;
    Shader m_skyboxShader { "./res/shaders/Skybox.shader" };
};

Application* GetApplication()
{
    return new MyApp();
}

}
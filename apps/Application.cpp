#include "DebugPanel.hpp"
#include "Panel.hpp"
#include "PropertyPanel.hpp"
#include "RenderStatsPanel.hpp"
#include "SceneHierarchyPanel.hpp"
#include "ViewportPanel.hpp"

// peanut
#include "peanut/FileSelectorDialog.hpp"
#include "peanut/FrameBuffer.hpp"
#include "peanut/RenderBuffer.hpp"
#include "peanut/Texture.hpp"
#include "peanut/WindowEvents.hpp"
#include <peanut/Application.hpp>
#include <peanut/Engine.hpp>
#include <peanut/Input.hpp>

// external
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// stl
#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace PEANUT {
class Entity;
}
struct GLFWwindow;

namespace PEANUT {

void UpdatePropertiesPanel(Entity selectedEntity, Engine* engine);

class MyApp : public Application {
public:
    void OnAttach() override
    {
        auto vp_panel = std::make_unique<ViewportPanel>(m_engine, &viewport_tex_);
        m_viewportPanel = vp_panel.get();
        m_panels.emplace_back(std::move(vp_panel));
        auto sh_panel = std::make_unique<SceneHierarchyPanel>(m_engine);
        m_panels.emplace_back(std::make_unique<PropertyPanel>(m_engine, sh_panel.get()));
        m_panels.emplace_back(std::move(sh_panel));
        m_panels.emplace_back(std::make_unique<RenderStatsPanel>(m_engine));
        m_panels.emplace_back(std::make_unique<DebugPanel>(m_engine));

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

        OnWindowResize(WindowResizeEvent { m_engine->GetWindow().GetWidth(), m_engine->GetWindow().GetHeight() });
        m_engine->SetViewport(m_viewportPanel->GetWidth(), m_viewportPanel->GetHeight());
    }

    void OnPreUpdate() override
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        framebuffer_.Bind();
        OnImGuiUpdate();
        AdjustRenderViewport(m_viewportPanel->GetWidth(), m_viewportPanel->GetHeight());
    }

    void OnUpdate(double timeStep) override
    {
        if (m_viewportPanel->IsFocused()) {
            UpdateCameraPosition(timeStep);
        }
    }

    void OnPostUpdate() override
    {
        framebuffer_.Unbind();
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
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

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

        for (const auto& panel : m_panels) {
            panel->Begin();
            panel->Update();
            panel->End();
        }

        ImGui::End();
    }

    void OnEvent(Event& event) override
    {
        Dispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>([&](const WindowResizeEvent& e) { OnWindowResize(e); });
        if (m_viewportPanel->IsHovered()) {
            dispatcher.Dispatch<ScrollEvent>([&](const ScrollEvent& e) { OnScroll(e); });
            dispatcher.Dispatch<MouseButtonEvent>([&](const MouseButtonEvent& e) { OnMouseButton(e); });
            dispatcher.Dispatch<MouseMovedEvent>([&](const MouseMovedEvent& e) { OnMouseMove(e); });
        }
        dispatcher.Dispatch<KeyEvent>([&](const KeyEvent& e) { OnKey(e); });
    }

private:
    void AdjustRenderViewport(float width, float height) { m_engine->GetCamera().SetAspectRatio(width, height); }

    void OnWindowResize(const WindowResizeEvent& e)
    {
        auto rb_config = render_buf_.GetConfig();
        rb_config.width_ = e.GetWidth();
        rb_config.height_ = e.GetHeight();
        render_buf_ = RenderBuffer { rb_config };

        auto tex_config = viewport_tex_.GetConfig();
        tex_config.width_ = e.GetWidth();
        tex_config.height_ = e.GetHeight();
        viewport_tex_ = Texture { tex_config };

        framebuffer_ = FrameBuffer({ &render_buf_, &viewport_tex_ });
        m_engine->SetViewport(e.GetWidth(), e.GetHeight());
    }

    void OnScroll(const ScrollEvent&) { }

    void OnMouseButton(const MouseButtonEvent& event)
    {
        m_leftMousePressed = event.GetButton() == MouseCode::MOUSE_BUTTON_LEFT && event.Pressed();
        m_rightMousePressed = event.GetButton() == MouseCode::MOUSE_BUTTON_RIGHT && event.Pressed();
    }

    void OnMouseMove(const MouseMovedEvent& event)
    {
        const glm::vec2 newPos(event.HorizontalPosition(), event.VerticalPosition());
        glm::vec2 diff = newPos - m_mousePosition;
        diff /= 0.5 * m_engine->GetWindow().GetHeight(); // Scales from 0 to pixelWidth to -1.0 to 1.0
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
                        const auto& plugins = m_engine->GetPlugins();
                        std::vector<std::string> plugin_names(plugins.size());
                        std::transform(plugins.cbegin(), plugins.cend(), plugin_names.begin(),
                            [](const auto& val) { return val.name; });
                        m_engine->Serialize(*m_engine->GetScene(), saveFile, plugin_names);
                    } else {
                        LOG_WARN("Failed to select save file. Not saving scene.");
                    }
                }
                if (ImGui::MenuItem("Open")) {
                    std::string sceneFile = CreateFileSelectorDialog()->OpenFile().value_or("");
                    if (sceneFile.find(".peanut") != std::string::npos) {
                        LOG_INFO("Opening Scene: {}", sceneFile);
                        const auto& plugins = m_engine->GetPlugins();
                        std::vector<std::string> plugin_names(plugins.size());
                        std::transform(plugins.cbegin(), plugins.cend(), plugin_names.begin(),
                            [](const auto& val) { return val.name; });
                        m_engine->Deserialize(*m_engine->GetScene(), sceneFile, plugin_names);
                        m_engine->GetScene()->ForEachEntity([this](Entity ent) {
                            if (ent.Has<PythonScriptComponent>()) {
                                m_engine->LoadPythonScriptObj(ent);
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

    std::vector<std::unique_ptr<Panel>> m_panels;
    ViewportPanel* m_viewportPanel;
    FrameBuffer framebuffer_ { {} };
    RenderBuffer render_buf_ { RenderBuffer::Config { .width_ = 100, .height_ = 100 } };
    Texture viewport_tex_ { Texture::Config { .width_ = 100, .height_ = 100, .filter_ = Texture::Filter::LINEAR } };
    glm::vec2 m_mousePosition = { 0.0, 0.0 };
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
};

Application* GetApplication() { return new MyApp(); }

}
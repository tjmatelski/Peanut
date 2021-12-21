#include "Settings.h"
#include <Application.h>
#include <KeyCodes.h>
#include <Log.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace PEANUT {
Application* Application::s_application = nullptr;
Application::Application()
{
    spdlog::set_level(spdlog::level::debug);

    s_application = this;
    m_window = std::make_unique<Window>("Peanut", 800, 600);
    m_window->SetEventCallback([this](Event& e) -> void { this->OnApplicationEvent(e); });
    Renderer2D::Init();
    Settings::Initialize();
}

void Application::Run()
{
    while (!m_shouldWindowClose) {
        float currentFrameTime = m_window->GetTime();
        TimeStep timeStep = currentFrameTime - m_lastFrameTime;
        m_lastFrameTime = currentFrameTime;

        ImGuiBeginFrame();

        OnImGuiUpdate();
        OnUpdate(timeStep);

        ImGuiEndFrame();

        UpdateWindow();
    }
}

void Application::UpdateWindow()
{
    m_window->SwapBuffers();
    m_window->PollEvents();
}

void Application::OnApplicationEvent(Event& event)
{
    Dispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>([this](const WindowCloseEvent& e) { this->OnWindowClose(e); });
    OnEvent(event);
}

void Application::OnWindowClose(const WindowCloseEvent& e)
{
    m_shouldWindowClose = true;
}

void Application::ImGuiBeginFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Application::ImGuiEndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::Terminate()
{
    m_shouldWindowClose = true;
}
} // namespace PEANUT

int main(int argc, char** argv)
{
    auto* app = PEANUT::GetApplication();
    app->OnAttach();
    LOG_INFO("Running Application");
    app->Run();
    app->OnRemove();

    return 0;
}
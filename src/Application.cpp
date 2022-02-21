#include "Settings.h"
#include <Application.h>
#include <KeyCodes.h>
#include <Log.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>

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

        OnUpdate(timeStep);

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
    dispatcher.Dispatch<WindowCloseEvent>([=]([[maybe_unused]] const auto& e) {
        Terminate();
    });
    OnEvent(event);
}

void Application::Terminate()
{
    m_shouldWindowClose = true;
}
} // namespace PEANUT

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    auto* app = PEANUT::GetApplication();
    app->OnAttach();
    LOG_INFO("Running Application");
    app->Run();
    app->OnRemove();

    return 0;
}
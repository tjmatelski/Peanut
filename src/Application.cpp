#include <Application.h>

#include "Settings.h"
#include <KeyCodes.h>
#include <Log.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>

#include <sol/sol.hpp>

namespace PEANUT {
Application* Application::s_application = nullptr;
Application::Application()
{
    spdlog::set_level(spdlog::level::debug);

    s_application = this;
    m_window = std::make_unique<Window>("Peanut", 800, 600);
    m_window->SetEventCallback([this](Event& e) -> void { this->OnApplicationEvent(e); });
    Renderer2D::Init();
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
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
    LOG_INFO("Starting Application: {}", argv[0]);
    PEANUT::Settings::SetApplication(argv[0]);

    auto* app = PEANUT::GetApplication();
    app->OnAttach();
    LOG_INFO("Running Application");
    app->Run();
    app->OnRemove();

    return 0;
}
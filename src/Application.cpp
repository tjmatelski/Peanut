#include <Application.h>
#include <Log.h>

namespace PEANUT
{
    Application::Application()
    {
        m_window = std::make_unique<Window>("Peanut", 800, 600);
        m_window->SetEventCallback([this] (Event& e) -> void { this->OnEvent(e); } );
    }

    void Application::Run()
    {
        while (!m_shouldWindowClose)
        {
            OnUpdate();
            UpdateWindow();
        }
    }

    void Application::UpdateWindow()
    {
        m_window->SwapBuffers();
        m_window->PollEvents();
        m_shouldWindowClose = m_window->WindowShouldClose();
    }

    void Application::OnEvent(Event& event)
    {
        LOG_INFO("Application Event: {0}", static_cast<int>(event.GetType()));
        Dispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>([this] (WindowCloseEvent& e) { this->OnWindowClose(e); });
    }

    void Application::OnWindowClose(WindowCloseEvent& e)
    {
        LOG_INFO("Registered Window Should Close Event");
        m_shouldWindowClose = true;
    }
}

int main(int argc, char** argv)
{
    auto* app = PEANUT::GetApplication();
    app->OnAttach();
    LOG_INFO("Running Application");
    app->Run();
    app->OnRemove();

    return 0;
}
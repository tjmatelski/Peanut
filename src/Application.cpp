#include <Application.h>
#include <Log.h>
#include <KeyCodes.h>
#include <Renderer/Renderer.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace PEANUT
{
    Application::Application()
    {
        m_window = std::make_unique<Window>("Peanut", 800, 600);
        m_window->SetEventCallback([this](Event &e) -> void { this->OnEvent(e); });
    }

    void Application::Run()
    {
        while (!m_shouldWindowClose)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.
            ImGui::End();

            OnUpdate();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            UpdateWindow();

        }
    }

    void Application::UpdateWindow()
    {
        m_window->SwapBuffers();
        m_window->PollEvents();
    }

    void Application::OnEvent(Event &event)
    {
        Dispatcher dispatcher(event);
        dispatcher.Dispatch<KeyEvent>([this](KeyEvent &e) { this->OnKeyEvent(e); });
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent &e) { this->OnWindowClose(e); });
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent &e) { this->OnWindowResize(e); });
    }

    void Application::OnWindowClose(WindowCloseEvent &e)
    {
        m_shouldWindowClose = true;
    }

    void Application::OnWindowResize(WindowResizeEvent &e)
    {
        Renderer::SetViewport(e.GetWidth(), e.GetHeight());
    }

    void Application::OnKeyEvent(KeyEvent &e)
    {
        switch (e.GetCode())
        {
        case KeyCode::ESCAPE:
            m_shouldWindowClose = true;
            break;

        default:
            break;
        }
    }
} // namespace PEANUT

int main(int argc, char **argv)
{
    auto *app = PEANUT::GetApplication();
    app->OnAttach();
    LOG_INFO("Running Application");
    app->Run();
    app->OnRemove();

    return 0;
}
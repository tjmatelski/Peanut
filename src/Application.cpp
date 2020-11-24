#include <Application.h>
#include <Log.h>

namespace PEANUT
{
    void Application::Run()
    {
        while (!m_shouldWindowClose)
        {
            OnUpdate();
        }
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
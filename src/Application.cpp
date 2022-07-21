#include <Application.h>

#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Settings.h"
#include "TimeStep.h"
#include <KeyCodes.h>
#include <Log.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>

#include <filesystem>
#include <sol/sol.hpp>

namespace PEANUT {
Application* Application::s_application = nullptr;
Application::Application()
    : m_scene(std::make_shared<Scene>())
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

        if (m_runtime) {
            UpdateLuaScripts(timeStep);
        }

        UpdateWindow();
    }
}

void Application::UpdateWindow()
{
    m_window->SwapBuffers();
    m_window->PollEvents();
}

void Application::UpdateLuaScripts(TimeStep ts)
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.new_usertype<TransformComponent>("TransformComponent",
        "position", &TransformComponent::translation,
        "rotation", &TransformComponent::rotation,
        "scale", &TransformComponent::scale);
    lua.new_usertype<glm::vec3>("glm::vec3",
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z);

    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<LuaScriptComponent>()) {
            lua["transform"] = &ent.Get<TransformComponent>();
            const auto& scriptComp = ent.Get<LuaScriptComponent>();
            lua.script_file(std::filesystem::absolute(scriptComp.script));
            lua["OnUpdate"](ts);
        }
    });
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
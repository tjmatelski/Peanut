#pragma once

#include <Events/Event.h>
#include <Renderer/PerspectiveCamera.h>
#include <Renderer/Shader.h>
#include <Scene/Scene.h>
#include <Window.h>

#include <memory>

int main(int argc, char** argv);

namespace PEANUT {
class Application;
class Engine {
public:
    Engine();
    ~Engine();

    static auto Get() -> Engine&;

    void StartRuntime() { m_runtime = true; }
    void StopRunTime() { m_runtime = false; }
    void Terminate();

    [[nodiscard]] auto GetWindow() const -> const Window& { return *m_window; }
    [[nodiscard]] auto GetScene() const -> std::shared_ptr<Scene> { return m_scene; }
    [[nodiscard]] auto IsRuntime() const -> bool { return m_runtime; }
    [[nodiscard]] auto GetCamera() -> PerspectiveCamera& { return m_perspectiveCam; }

private:
    Application* m_app = nullptr;
    std::unique_ptr<Window> m_window;
    std::shared_ptr<Scene> m_scene;
    std::unique_ptr<Shader> m_lightingShader;
    std::unique_ptr<Shader> m_skyboxShader;
    PerspectiveCamera m_perspectiveCam = { { 0.0, 0.0, 0.0 } };
    double m_lastFrameTime = 0.0;
    bool m_shouldWindowClose = false;
    bool m_runtime = false;

    void Run();
    void OnApplicationEvent(Event& event);
    void UpdateWindow();
    void UpdateLuaScripts(double dt);
    void Update(double dt);

    friend int ::main(int argc, char** argv);
};

} // namespace PEANUT
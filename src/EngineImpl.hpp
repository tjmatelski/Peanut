#pragma once

#include "Renderer/Shader.hpp"
#include <peanut/Component.hpp>
#include <peanut/Entity.hpp>
#include <peanut/Event.hpp>
#include <peanut/PerspectiveCamera.hpp>
#include <peanut/PluginManager.hpp>
#include <peanut/Scene.hpp>
#include <peanut/Window.hpp>

// stl
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

int main(int argc, char** argv);

namespace PEANUT {
class PythonScript;
class Application;
class EngineImpl {
public:
    EngineImpl();
    ~EngineImpl();

    static auto Get() -> EngineImpl&;

    [[nodiscard]] auto GetWindow() const -> const Window& { return *m_window; }
    [[nodiscard]] auto GetScene() const -> std::shared_ptr<Scene> { return m_scene; }
    [[nodiscard]] auto IsRuntime() const -> bool { return m_runtime; }
    [[nodiscard]] auto GetCamera() -> PerspectiveCamera& { return m_perspectiveCam; }
    [[nodiscard]] auto GetPlugins() -> const std::vector<Plugin>& { return m_pluginManager.Plugins(); }
    void StartRuntime();
    void StopRunTime();
    void Terminate();
    void ReloadPlugin(std::string_view name);
    void SetViewport(int width, int height);
    void Serialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins);
    void Deserialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins);

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
    PluginManager m_pluginManager;

    void Run();
    void OnApplicationEvent(Event& event);
    void UpdateWindow();
    void BeginRuntime();
    void UpdateRuntimeScripts(double dt);
    void EndRuntime();
    void Update(double dt);

    friend int ::main(int argc, char** argv);
};

} // namespace PEANUT
#pragma once

#include "peanut/Mesh.hpp"
#include <peanut/Component.hpp>
#include <peanut/Engine.hpp>
#include <peanut/Entity.hpp>
#include <peanut/Event.hpp>
#include <peanut/FrameBuffer.hpp>
#include <peanut/PerspectiveCamera.hpp>
#include <peanut/PluginManager.hpp>
#include <peanut/Scene.hpp>
#include <peanut/Shader.hpp>
#include <peanut/Window.hpp>

// stl
#include <memory>
#include <string>

int main(int argc, char** argv);

namespace PEANUT {
class PythonScript;
class Application;
class EditorFieldMap;

class EngineImpl {
public:
    EngineImpl();
    ~EngineImpl();

    static auto Get() -> EngineImpl&;

    [[nodiscard]] auto GetWindow() const -> const Window& { return m_window; }
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
    Window m_window;
    std::shared_ptr<Scene> m_scene;
    PerspectiveCamera m_perspectiveCam = { { 0.0, 0.0, 0.0 } };
    double m_lastFrameTime = 0.0;
    bool m_shouldWindowClose = false;
    bool m_runtime = false;
    PluginManager m_pluginManager;
    std::unique_ptr<FrameBuffer> m_shadow_fb;
    Renderable m_quad;
    int m_viewport_width;
    int m_viewport_height;

    void Run();
    void OnApplicationEvent(Event& event);
    void UpdateWindow();
    void BeginRuntime();
    void UpdateRuntimeScripts(double dt);
    void EndRuntime();
    void Update(double dt);

    friend int ::main(int argc, char** argv);
};

Mesh GetCubeMesh();
void LoadPythonScriptObj(Entity ent);
void ReloadPythonScript(Entity ent);
Engine::EditorFieldMap& GetScriptEditorMembers(PythonScript* script);

} // namespace PEANUT
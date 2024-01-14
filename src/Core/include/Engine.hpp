#pragma once

#include "PluginManager.hpp"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include <Events/Event.h>
#include <Renderer/PerspectiveCamera.h>
#include <Renderer/Shader.h>
#include <Scene/Scene.h>
#include <Window.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

int main(int argc, char** argv);

namespace PEANUT {
class PythonScript;
class Application;
class Engine {
public:
    Engine();
    ~Engine();

    static auto Get() -> Engine&;

    void StartRuntime();
    void StopRunTime();
    void Terminate();

    [[nodiscard]] auto GetWindow() const -> const Window& { return *m_window; }
    [[nodiscard]] auto GetScene() const -> std::shared_ptr<Scene> { return m_scene; }
    [[nodiscard]] auto IsRuntime() const -> bool { return m_runtime; }
    [[nodiscard]] auto GetCamera() -> PerspectiveCamera& { return m_perspectiveCam; }
    [[nodiscard]] auto GetPlugins() -> const std::vector<Plugin>& { return m_pluginManager.Plugins(); }

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

void LoadPythonScriptObj(Entity ent);
void ReloadPythonScript(Entity ent);

struct EditorButton {
    bool pressed = false;
};
using EditorFieldMap = std::unordered_map<std::string, std::variant<int, float, EditorButton>>;
EditorFieldMap& GetScriptEditorMembers(PythonScript* script);

// Custom Model/Mesh
void CreateCustomModel(const CustomModelComponent& model);
Mesh GetCubeMesh();

} // namespace PEANUT
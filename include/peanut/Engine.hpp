#pragma once

#include <peanut/Component.hpp>
#include <peanut/Entity.hpp>
#include <peanut/PerspectiveCamera.hpp>
#include <peanut/PluginManager.hpp>
#include <peanut/Scene.hpp>
#include <peanut/Window.hpp>

// stl
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

int main(int argc, char** argv);

namespace PEANUT {
class PythonScript;
class Application;
class EngineImpl;
class Window;
class PerspectiveCamera;

class Engine {
public:
    struct EditorButton {
        bool pressed = false;
    };
    using EditorFieldMap = std::unordered_map<std::string, std::variant<int, float, EditorButton>>;

    [[nodiscard]] auto GetWindow() const -> const Window&;
    [[nodiscard]] auto GetScene() const -> std::shared_ptr<Scene>;
    [[nodiscard]] auto IsRuntime() const -> bool;
    [[nodiscard]] auto GetCamera() -> PerspectiveCamera&;
    [[nodiscard]] auto GetPlugins() -> const std::vector<Plugin>&;
    [[nodiscard]] auto GetCubeMesh() -> Mesh;
    [[nodiscard]] auto GetScriptEditorMembers(PythonScript* script) -> EditorFieldMap&;

    void StartRuntime();
    void StopRunTime();
    void Terminate();
    void ReloadPlugin(std::string_view name);
    void SetViewport(int width, int height);
    void Serialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins);
    void Deserialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins);
    void LoadPythonScriptObj(Entity ent);
    void ReloadPythonScript(Entity ent);
    void RedrawMesh(const CustomModelComponent& model);

private:
    EngineImpl* m_engine = nullptr;
    friend int ::main(int argc, char** argv);
};

} // namespace PEANUT
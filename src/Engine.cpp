#include "EngineImpl.hpp"
#include <peanut/Engine.hpp>

namespace PEANUT {

[[nodiscard]] auto Engine::GetWindow() const -> const Window& { return m_engine->GetWindow(); }

[[nodiscard]] auto Engine::GetScene() const -> std::shared_ptr<Scene> { return m_engine->GetScene(); }

[[nodiscard]] auto Engine::IsRuntime() const -> bool { return m_engine->IsRuntime(); }

[[nodiscard]] auto Engine::GetCamera() -> PerspectiveCamera& { return m_engine->GetCamera(); }

[[nodiscard]] auto Engine::GetPlugins() -> const std::vector<Plugin>& { return m_engine->GetPlugins(); }

void Engine::StartRuntime() { m_engine->StartRuntime(); }

void Engine::StopRunTime() { m_engine->StopRunTime(); }

void Engine::Terminate() { m_engine->Terminate(); }

void Engine::ReloadPlugin(std::string_view name) { m_engine->ReloadPlugin(name); }

void Engine::SetViewport(int width, int height) { m_engine->SetViewport(width, height); }

void Engine::Serialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins)
{
    m_engine->Serialize(scene, file, plugins);
}

void Engine::Deserialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins)
{
    m_engine->Deserialize(scene, file, plugins);
}

} // namespace PEANUT
#include "Input/Input.h"
#include "Renderer/IndexBuffer.h"
#include <Engine.hpp>

#include <Application.h>
#include <Events/WindowEvents.h>
#include <Input/Input.h>
#include <Input/KeyCodes.h>
#include <Input/MouseCodes.h>
#include <Renderer/ModelLibrary.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/Shader.h>
#include <Renderer/TextureLibrary.h>
#include <Scene/Component.h>
#include <Scene/Entity.h>
#include <Utils/Log.h>
#include <Utils/Settings.h>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <filesystem>
#include <memory>
#include <utility>

namespace PEANUT {
Engine::Engine()
    : m_scene(std::make_shared<Scene>())
{
    spdlog::set_level(spdlog::level::trace);

    m_window = std::make_unique<Window>("Peanut", 800, 600);
    m_window->SetEventCallback([this](Event& e) -> void { this->OnApplicationEvent(e); });
    Renderer2D::Init();
    Renderer::EnableDepthTest();

    m_lightingShader = std::make_unique<Shader>("./res/shaders/Lighting.shader");
    m_skyboxShader = std::make_unique<Shader>("./res/shaders/Skybox.shader");
}

Engine::~Engine()
{
    delete m_app;
}

auto Engine::Get() -> Engine&
{
    static Engine engine;
    return engine;
}

void Engine::Run()
{
    m_app->OnAttach();
    while (!m_shouldWindowClose) {
        double currentFrameTime = m_window->GetTime();
        double timeStep = currentFrameTime - m_lastFrameTime;
        m_lastFrameTime = currentFrameTime;

        m_app->OnPreUpdate();
        m_app->OnUpdate(timeStep);
        Update(timeStep);
        m_app->OnPostUpdate();

        if (m_runtime) {
            UpdateRuntimeScripts(timeStep);
        }

        UpdateWindow();
    }
    m_app->OnRemove();
}

void Engine::Update(double dt)
{
    // Clear buffers for start of frame
    Renderer::ClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    Renderer::ClearBuffers();

    // Render skybox
    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<SkyboxComponent>()) {
            m_skyboxShader->SetUniformMat4("view", glm::mat4(glm::mat3(m_perspectiveCam.GetViewMatrix())));
            m_skyboxShader->SetUniformMat4("projection", m_perspectiveCam.GetProjectionMatrix());
            const auto& skybox = ent.Get<SkyboxComponent>();
            Renderer::DisableDepthMask();
            Renderer::Draw(Renderer::GetSkyboxMesh(), Material({ TextureLibrary::Load(skybox.directory, Texture::Type::CubeMap) }), *m_skyboxShader);
            Renderer::EnableDepthMask();
        }
    });

    // Render 2D sprites
    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<SpriteRenderComponent>()) {
            const auto& spriteRender = ent.Get<SpriteRenderComponent>();
            Renderer2D::DrawQuad(ent.Get<TransformComponent>(), spriteRender.color, TextureLibrary::Load(spriteRender.texture));
        }
    });

    // Render Directional Lights
    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<DirectionalLightComponent>()) {
            auto& comp = ent.Get<DirectionalLightComponent>();
            Renderer::SetDirectionalLight(
                { comp.direction,
                    { comp.ambient, comp.ambient, comp.ambient },
                    { comp.diffuse, comp.diffuse, comp.diffuse },
                    { comp.specular, comp.specular, comp.specular } },
                *m_lightingShader);
        }
    });

    // Render Point Lights
    std::vector<PointLight> pointLights;
    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<PointLightComponent>()) {
            auto& comp = ent.Get<PointLightComponent>();
            PointLight pl;
            pl.active = true;
            pl.position = ent.Get<TransformComponent>().translation;
            pl.ambient = comp.ambient * comp.color;
            pl.diffuse = comp.diffuse * comp.color;
            pl.specular = comp.specular * comp.color;
            pl.constant = comp.constant;
            pl.linear = comp.linear;
            pl.quadratic = comp.quadratic;
            pointLights.push_back(pl);
        }
    });
    Renderer::SetPointLights(pointLights, *m_lightingShader);

    // Render Models
    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<ModelFileComponent>()) {
            m_lightingShader->SetUniformMat4("view", m_perspectiveCam.GetViewMatrix());
            m_lightingShader->SetUniformMat4("projection", m_perspectiveCam.GetProjectionMatrix());
            m_lightingShader->SetUniformVec3("viewPos", m_perspectiveCam.Position());
            m_lightingShader->SetUniformMat4("model", ent.Get<TransformComponent>());
            Renderer::Draw(ModelLibrary::Get(ent.Get<ModelFileComponent>().file), *m_lightingShader);
        }
    });
}

void Engine::UpdateWindow()
{
    m_window->SwapBuffers();
    m_window->PollEvents();
}

void UpdatePythonScripts(double dt, Entity ent, const std::filesystem::path& script);

void Engine::UpdateRuntimeScripts(double ts)
{
    pybind11::scoped_interpreter python;
    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<PythonScriptComponent>()) {
            UpdatePythonScripts(ts, ent, ent.Get<PythonScriptComponent>().script);
        }
    });
}

void Engine::OnApplicationEvent(Event& event)
{
    Dispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>([=]([[maybe_unused]] const auto& e) {
        Terminate();
    });
    m_app->OnEvent(event);
}

void Engine::Terminate()
{
    m_shouldWindowClose = true;
}
} // namespace PEANUT

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
    LOG_INFO("Starting Application: {}", argv[0]);
    PEANUT::Settings::SetApplication(argv[0]);

    auto& engine = PEANUT::Engine::Get();
    engine.m_app = PEANUT::GetApplication();
    engine.m_app->m_engine = &engine;
    engine.Run();

    return 0;
}
#include <Engine.hpp>

#include "PythonBindings.hpp"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include <Application.h>
#include <Events/WindowEvents.h>
#include <Input/Input.h>
#include <Input/KeyCodes.h>
#include <Input/MouseCodes.h>
#include <Renderer/IndexBuffer.h>
#include <Renderer/ModelLibrary.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/Shader.h>
#include <Renderer/TextureLibrary.h>
#include <Scene/Component.h>
#include <Scene/Entity.h>
#include <Utils/Log.h>
#include <Utils/Settings.h>

#include <cstddef>
#include <exception>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <filesystem>
#include <memory>
#include <pybind11/pytypes.h>
#include <unordered_map>
#include <utility>

namespace PEANUT {

namespace {
    std::unordered_map<unsigned int, Renderable> customModels;
}

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

    LOG_DEBUG("Initializing Python Interpreter");
    pybind11::initialize_interpreter();
}

Engine::~Engine()
{
    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<PythonScriptComponent>()) {
            const auto comp = ent.Get<PythonScriptComponent>();
            py::cast(comp.script_obj).dec_ref();
        }
    });
    LOG_DEBUG("Stopping Python Interpreter");
    pybind11::finalize_interpreter();
    delete m_app;
}

auto Engine::Get() -> Engine&
{
    static Engine engine;
    return engine;
}

void Engine::Run()
{
    m_pluginManager.LoadAll(Settings::GetApplicationDir());
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
    if (m_runtime) {
        EndRuntime();
    }
    m_app->OnRemove();
}

void Engine::StartRuntime()
{
    m_runtime = true;
    BeginRuntime();
}

void Engine::StopRunTime()
{
    m_runtime = false;
    EndRuntime();
}

void Engine::Update(double dt)
{
    try {
        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<PythonScriptComponent>()) {
                const auto comp = ent.Get<PythonScriptComponent>();
                comp.script_obj->editor_update();
            }
        });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
    }

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

    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<CustomModelComponent>()) {
            m_lightingShader->SetUniformMat4("view", m_perspectiveCam.GetViewMatrix());
            m_lightingShader->SetUniformMat4("projection", m_perspectiveCam.GetProjectionMatrix());
            m_lightingShader->SetUniformVec3("viewPos", m_perspectiveCam.Position());
            m_lightingShader->SetUniformMat4("model", ent.Get<TransformComponent>());
            const auto& renderable = customModels.at(ent.Get<CustomModelComponent>().id);
            Renderer::Draw(renderable.mesh, renderable.material, *m_lightingShader);
        }
    });
}

void Engine::UpdateWindow()
{
    m_window->SwapBuffers();
    m_window->PollEvents();
}

void Engine::BeginRuntime()
{
    try {
        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<PythonScriptComponent>()) {
                const auto comp = ent.Get<PythonScriptComponent>();
                comp.script_obj->runtime_begin();
            }
        });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
    }
}

void Engine::UpdateRuntimeScripts(double ts)
{
    bool python_error = false;
    try {
        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<PythonScriptComponent>()) {
                const auto comp = ent.Get<PythonScriptComponent>();
                comp.script_obj->update(ts);
            }
        });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
        python_error = true;
    }
    if (python_error) {
        StopRunTime();
    }
}

void Engine::EndRuntime()
{
    try {
        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<PythonScriptComponent>()) {
                const auto comp = ent.Get<PythonScriptComponent>();
                comp.script_obj->runtime_end();
            }
        });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
    }
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

void LoadPythonScriptObj(Entity ent)
{
    // Get Component parts
    auto& comp = ent.Get<PythonScriptComponent>();
    const auto& script = comp.script;

    // Load module
    auto sys = pybind11::module_::import("sys");
    sys.attr("path").attr("append")(script.parent_path().c_str());
    auto module = pybind11::module_::import(script.stem().c_str());
    module.reload();

    // Create instance
    auto instance = module.attr(script.stem().c_str())();
    instance.inc_ref(); // So the python interpreter keeps object alive while C++ has ownership.
    comp.script_obj = instance.cast<PythonScript*>();
    comp.script_obj->m_ent = ent;
}

void ReloadPythonScript(Entity ent)
{
    auto& comp = ent.Get<PythonScriptComponent>();
    auto inst = py::cast(comp.script_obj);
    inst.dec_ref();
    comp.script_obj = nullptr;
    LoadPythonScriptObj(ent);
}

EditorFieldMap& GetScriptEditorMembers(PythonScript* script)
{
    return script->editor_fields;
}

void CreateCustomModel(const CustomModelComponent& model)
{
    customModels.emplace(model.id, Renderable { OpenglMesh { model.mesh.vertices, model.mesh.indices }, Material { { Texture {} } } });
}

Mesh GetCubeMesh()
{
    return Renderer::GetCubeMesh();
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
#include "EngineImpl.hpp"

#include "PythonBindings.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/TextureLibrary.hpp"
#include "SceneSerializer.hpp"
#include "Settings.hpp"
#include "peanut/Engine.hpp"
#include <peanut/Application.hpp>
#include <peanut/Component.hpp>
#include <peanut/Entity.hpp>
#include <peanut/Input.hpp>
#include <peanut/KeyCodes.hpp>
#include <peanut/Log.hpp>
#include <peanut/Material.hpp>
#include <peanut/Mesh.hpp>
#include <peanut/ModelLibrary.hpp>
#include <peanut/MouseCodes.hpp>
#include <peanut/NativeScript.hpp>
#include <peanut/Shader.hpp>
#include <peanut/ShaderLibrary.hpp>
#include <peanut/WindowEvents.hpp>

// external
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// stl
#include <exception>
#include <pybind11/pytypes.h>
#include <spdlog/spdlog.h>

namespace PEANUT {

EngineImpl::EngineImpl()
    : m_scene(std::make_shared<Scene>())
{
    spdlog::set_level(spdlog::level::trace);

    m_window = std::make_unique<Window>("Peanut", 800, 600);
    m_window->SetEventCallback([this](Event& e) -> void { this->OnApplicationEvent(e); });
    Renderer2D::Init();
    Renderer::EnableDepthTest();

    LOG_DEBUG("Initializing Python Interpreter");
    pybind11::initialize_interpreter();
}

EngineImpl::~EngineImpl()
{
    m_scene->ForEach<PythonScriptComponent>(
        [](Entity, const PythonScriptComponent& comp) { py::cast(comp.script_obj).dec_ref(); });
    LOG_DEBUG("Stopping Python Interpreter");
    pybind11::finalize_interpreter();
    Renderer2D::Destroy();
    delete m_app;
}

auto EngineImpl::Get() -> EngineImpl&
{
    static EngineImpl engine;
    return engine;
}

void EngineImpl::Run()
{
    m_pluginManager.LoadAll(Settings::GetApplicationDir() / "plugins");
    m_app->OnAttach();
    while (!m_shouldWindowClose) {
        double currentFrameTime = m_window->GetTime();
        double timeStep = currentFrameTime - m_lastFrameTime;
        m_lastFrameTime = currentFrameTime;

        m_app->OnPreUpdate();
        m_app->OnUpdate(timeStep);
        for (auto& plugin : m_pluginManager.Plugins()) {
            m_scene->ForEachEntity([&](Entity ent) {
                if (ent.Has<NativeScript>(plugin.name)) {
                    ent.Get<NativeScript>(plugin.name)->EditorUpdate();
                }
            });
        }
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

void EngineImpl::StartRuntime()
{
    m_runtime = true;
    BeginRuntime();
}

void EngineImpl::StopRunTime()
{
    m_runtime = false;
    EndRuntime();
}

void EngineImpl::Update(double)
{
    try {
        m_scene->ForEach<PythonScriptComponent>(
            [](Entity, const PythonScriptComponent& comp) { comp.script_obj->editor_update(); });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
    }

    // Clear buffers for start of frame
    Renderer::ClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    Renderer::ClearBuffers();

    // Render skybox
    m_scene->ForEach<SkyboxComponent>([&](Entity, const SkyboxComponent& skybox) {
        ShaderLibrary::Get("./res/shaders/Skybox.shader")
            ->SetUniform({ "view", glm::mat4(glm::mat3(m_perspectiveCam.GetViewMatrix())) });
        ShaderLibrary::Get("./res/shaders/Skybox.shader")
            ->SetUniform({ "projection", m_perspectiveCam.GetProjectionMatrix() });
        Renderer::DisableDepthMask();
        Material mat;
        mat.AddTexture(TextureLibrary::Load(skybox.directory, Texture::Type::CubeMap));
        const Renderable renderable = { .mesh_ = Renderer::GetSkyboxMesh(),
            .material_ = mat,
            .shader_ = ShaderLibrary::Get("./res/shaders/Skybox.shader") };
        Renderer::Draw(renderable);
        Renderer::EnableDepthMask();
    });

    // Render 2D sprites
    m_scene->ForEach<SpriteRenderComponent>([&](Entity ent, const SpriteRenderComponent& spriteRender) {
        Renderer2D::DrawQuad(
            ent.Get<TransformComponent>(), spriteRender.color, TextureLibrary::Load(spriteRender.texture));
    });

    // Render Directional Lights
    m_scene->ForEach<DirectionalLightComponent>([&](Entity, const DirectionalLightComponent& comp) {
        Renderer::SetDirectionalLight(
            { comp.direction, { comp.ambient, comp.ambient, comp.ambient },
                { comp.diffuse, comp.diffuse, comp.diffuse }, { comp.specular, comp.specular, comp.specular } },
            *ShaderLibrary::Get("./res/shaders/Lighting.shader"));
    });

    // Render Point Lights
    std::vector<PointLight> pointLights;
    m_scene->ForEach<PointLightComponent>([&](Entity ent, const PointLightComponent& comp) {
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
    });
    Renderer::SetPointLights(pointLights, *ShaderLibrary::Get("./res/shaders/Lighting.shader"));

    // Render Models
    m_scene->ForEach<ModelFileComponent>([&](Entity ent, const ModelFileComponent& comp) {
        // TODO: Should camera uniforms be handled here or in renderer
        auto& model = ModelLibrary::Get(comp.file);
        for (auto& renderable : model.GetRenderables()) {
            renderable.shader_->SetUniform({ "view", m_perspectiveCam.GetViewMatrix() });
            renderable.shader_->SetUniform({ "projection", m_perspectiveCam.GetProjectionMatrix() });
            renderable.shader_->SetUniform({ "viewPos", m_perspectiveCam.Position() });
            renderable.shader_->SetUniform({ "model", ent.Get<TransformComponent>() });
        }
        Renderer::Draw(model);
    });

    // Render Renderables
    m_scene->ForEach<Renderable>([&](Entity ent, const Renderable& renderable) {
        renderable.shader_->SetUniform({ "view", m_perspectiveCam.GetViewMatrix() });
        renderable.shader_->SetUniform({ "projection", m_perspectiveCam.GetProjectionMatrix() });
        renderable.shader_->SetUniform({ "viewPos", m_perspectiveCam.Position() });
        renderable.shader_->SetUniform({ "model", ent.Get<TransformComponent>() });
        Renderer::Draw(renderable);
    });
}

void EngineImpl::UpdateWindow()
{
    m_window->SwapBuffers();
    m_window->PollEvents();
}

void EngineImpl::BeginRuntime()
{
    for (auto& plugin : m_pluginManager.Plugins()) {
        m_scene->ForEachEntity([plugin](Entity ent) {
            if (ent.Has<NativeScript>(plugin.name)) {
                ent.Get<NativeScript>(plugin.name)->RuntimeBegin();
            }
        });
    }
    try {
        m_scene->ForEach<PythonScriptComponent>(
            [&](Entity, const PythonScriptComponent& comp) { comp.script_obj->runtime_begin(); });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
    }
}

void EngineImpl::UpdateRuntimeScripts(double ts)
{
    bool python_error = false;
    try {
        m_scene->ForEach<PythonScriptComponent>(
            [&](Entity, const PythonScriptComponent& comp) { comp.script_obj->update(ts); });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
        python_error = true;
    }
    if (python_error) {
        StopRunTime();
    }

    for (const auto& plugin : m_pluginManager.Plugins()) {
        m_scene->ForEachEntity([&](Entity ent) {
            if (ent.Has<NativeScript>(plugin.name)) {
                ent.Get<NativeScript>(plugin.name)->Update(ts);
            }
        });
    }
}

void EngineImpl::EndRuntime()
{
    for (auto& plugin : m_pluginManager.Plugins()) {
        m_scene->ForEachEntity([plugin](Entity ent) {
            if (ent.Has<NativeScript>(plugin.name)) {
                ent.Get<NativeScript>(plugin.name)->RuntimeEnd();
            }
        });
    }
    try {
        m_scene->ForEach<PythonScriptComponent>(
            [&](Entity, const PythonScriptComponent& comp) { comp.script_obj->runtime_end(); });
    } catch (std::exception& e) {
        LOG_ERROR("Python Script Threw Exception: {}", e.what());
    }
}

void EngineImpl::OnApplicationEvent(Event& event)
{
    Dispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>([this]([[maybe_unused]] const auto& e) { Terminate(); });
    m_app->OnEvent(event);
}

void EngineImpl::Terminate() { m_shouldWindowClose = true; }

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

Engine::EditorFieldMap& GetScriptEditorMembers(PythonScript* script) { return script->editor_fields; }

Mesh GetCubeMesh() { return Renderer::GetCubeMesh(); }

void EngineImpl::ReloadPlugin(std::string_view name) { m_pluginManager.Reload(name); }

void EngineImpl::SetViewport(int width, int height) { Renderer::SetViewport(width, height); }

void EngineImpl::Serialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins)
{
    SceneSerializer::Serialize(scene, file, plugins);
}

void EngineImpl::Deserialize(Scene& scene, const std::string& file, const std::vector<std::string>& plugins)
{
    SceneSerializer::Deserialize(file, scene, plugins);
}

} // namespace PEANUT

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto basic_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("peanut.log");
    std::vector<spdlog::sink_ptr> sinks { console_sink, basic_sink };
    auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
    spdlog::register_logger(logger); // if it would be used in some other place
    spdlog::set_default_logger(logger);
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));

    LOG_INFO("Starting Application: {}", argv[0]);
    PEANUT::Settings::SetApplication(argv[0]);

    PEANUT::Engine engine;
    auto& engine_impl = PEANUT::EngineImpl::Get();
    engine_impl.m_app = PEANUT::GetApplication();
    engine.m_engine = &engine_impl;
    engine_impl.m_app->m_engine = &engine;
    engine_impl.Run();

    return 0;
}
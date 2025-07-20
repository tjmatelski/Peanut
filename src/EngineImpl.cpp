#include "EngineImpl.hpp"

#include "PythonBindings.hpp"
#include "Renderer/GLDebug.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/TextureLibrary.hpp"
#include "SceneSerializer.hpp"
#include "Settings.hpp"
#include "peanut/Engine.hpp"
#include "peanut/FrameBuffer.hpp"
#include <algorithm>
#include <array>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <limits>
#include <memory>
#include <numeric>
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
#include <utility>

namespace PEANUT {

EngineImpl::EngineImpl()
    : m_window("Peanut", 1280, 720)
    , m_scene(std::make_shared<Scene>())
    // , m_shadow_fb(FrameBufferConfig { .width = 1024, .height = 1024, .type = FrameBufferConfig::Type::DEPTH })
    , m_quad(Renderable { .mesh_ = Renderer::GetQuadMesh(), .material_ = {}, .shader_ = nullptr })
    , m_viewport_width(0)
    , m_viewport_height(0)
{
    spdlog::set_level(spdlog::level::trace);

    m_window.SetEventCallback([this](Event& e) -> void { this->OnApplicationEvent(e); });
    Renderer2D::Init();
    Renderer::EnableDepthTest();
    // m_shadow_fb
    //     = FrameBuffer(FrameBufferConfig { .width = 1024, .height = 1024, .type = FrameBufferConfig::Type::DEPTH });
    m_shadow_fb = std::make_unique<FrameBuffer>(
        FrameBufferConfig { .width = 1024, .height = 1024, .type = FrameBufferConfig::Type::DEPTH });

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
        spdlog::set_level(debug_config_.log_level_);

        double currentFrameTime = m_window.GetTime();
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

    // Generate frustrum in world space
    std::array<glm::vec4, 8> frustrum_coords = {
        // clang-format off
        glm::vec4{-1.0f, -1.0f, -1.0f, 1.0},
        glm::vec4{-1.0f, -1.0f,  1.0f, 1.0},
        glm::vec4{-1.0f,  1.0f, -1.0f, 1.0},
        glm::vec4{-1.0f,  1.0f,  1.0f, 1.0},
        glm::vec4{ 1.0f, -1.0f, -1.0f, 1.0},
        glm::vec4{ 1.0f, -1.0f,  1.0f, 1.0},
        glm::vec4{ 1.0f,  1.0f, -1.0f, 1.0},
        glm::vec4{ 1.0f,  1.0f,  1.0f, 1.0},
        // clang-format on
    };
    const auto clip_to_world = glm::inverse(m_perspectiveCam.GetProjectionMatrix() * m_perspectiveCam.GetViewMatrix());
    std::ranges::for_each(frustrum_coords, [&clip_to_world](auto& coord) {
        coord = clip_to_world * coord;
        coord /= coord.w;
    });
    const glm::vec3 center
        = std::accumulate(frustrum_coords.begin(), frustrum_coords.end(), glm::vec4(0.0, 0.0, 0.0, 0.0))
        / static_cast<float>(frustrum_coords.size());
    glm::vec3 light_ortho_min(
        std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    glm::vec3 light_ortho_max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest());
    glm::vec3 dir_light_dir;
    m_scene->ForEach<DirectionalLightComponent>(
        [&](Entity, const DirectionalLightComponent& comp) { dir_light_dir = comp.direction; });
    glm::mat4 light_view = glm::lookAt(center - dir_light_dir, center, glm::vec3 { 0.0, 1.0, 0.0 });
    for (const auto& coord : frustrum_coords) {
        const auto coord_in_light_space = light_view * coord;
        light_ortho_min.x = std::min(light_ortho_min.x, coord_in_light_space.x);
        light_ortho_min.y = std::min(light_ortho_min.y, coord_in_light_space.y);
        light_ortho_min.z = std::min(light_ortho_min.z, coord_in_light_space.z);
        light_ortho_max.x = std::max(light_ortho_max.x, coord_in_light_space.x);
        light_ortho_max.y = std::max(light_ortho_max.y, coord_in_light_space.y);
        light_ortho_max.z = std::max(light_ortho_max.z, coord_in_light_space.z);
    }
    constexpr float zMult = 0.1f; // extend by 10% of distance
    const auto ortho_z_dist = light_ortho_min.z - light_ortho_max.z;
    light_ortho_min.z += ortho_z_dist * zMult;
    light_ortho_max.z -= ortho_z_dist * zMult;

    // Shadows
    struct ShadowConfig {
        int fb_width = 1024;
        int fb_height = 1024;
    };
    ShadowConfig config;
    Renderer::EnableDepthTest();
    GLint prev_fb;
    GLCALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fb));
    Renderer::SetViewport(config.fb_width, config.fb_width);
    m_shadow_fb->Bind();
    Renderer::ClearDepthBuffer();
    glm::mat4 lightProjection = glm::ortho(light_ortho_min.x, light_ortho_max.x, light_ortho_min.y, light_ortho_max.y,
        light_ortho_min.z, light_ortho_max.z);
    glm::mat4 lightSpaceMatrix = lightProjection * light_view;
    auto* depth_shader = ShaderLibrary::Get("./res/shaders/simpleDepth.shader");
    depth_shader->Use();
    depth_shader->SetUniform(Uniform { "lightSpaceMatrix", lightSpaceMatrix });
    // Render Models
    m_scene->ForEach<ModelFileComponent>([&](Entity ent, const ModelFileComponent& comp) {
        // TODO: Should camera uniforms be handled here or in renderer
        auto& model = ModelLibrary::Get(comp.file);
        depth_shader->SetUniform({ "model", ent.Get<TransformComponent>() });
        for (auto& renderable : model.GetRenderables()) {
            auto* actual_shader = renderable.shader_;
            renderable.shader_ = depth_shader;
            Renderer::Draw(renderable);
            renderable.shader_ = actual_shader;
        }
    });
    // Render Renderables
    m_scene->ForEach<Renderable>([&](Entity ent, Renderable& renderable) {
        depth_shader->SetUniform({ "model", ent.Get<TransformComponent>() });
        auto* actual_shader = std::exchange(renderable.shader_, depth_shader);
        Renderer::Draw(renderable);
        renderable.shader_ = actual_shader;
    });
    m_shadow_fb->Unbind();

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_fb));

    Renderer::SetViewport(m_window.GetWidth(), m_window.GetHeight());

    // Clear buffers for start of frame
    Renderer::ClearColor(0.2f, 0.2f, 0.2f, 0.1f);
    Renderer::ClearBuffers();

    // Render skybox
    m_scene->ForEach<SkyboxComponent>([&](Entity, const SkyboxComponent& skybox) {
        auto* p_shader = ShaderLibrary::Get("./res/shaders/Skybox.shader");
        p_shader->SetUniform({ "view", glm::mat4(glm::mat3(m_perspectiveCam.GetViewMatrix())) });
        p_shader->SetUniform({ "projection", m_perspectiveCam.GetProjectionMatrix() });
        Renderer::DisableDepthMask();
        const auto tex = TextureLibrary::GetCubemap(skybox.directory);
        static const Renderable renderable = { .mesh_ = Renderer::GetSkyboxMesh(),
            .material_ = {},
            .shader_ = ShaderLibrary::Get("./res/shaders/Skybox.shader") };
        GLCALL(glActiveTexture(GL_TEXTURE0));
        tex->Bind();
        Renderer::Draw(renderable);
        Renderer::EnableDepthMask();
    });

    // Render 2D sprites
    // m_scene->ForEach<SpriteRenderComponent>([&](Entity ent, const SpriteRenderComponent& spriteRender) {
    //     Renderer2D::DrawQuad(
    //         ent.Get<TransformComponent>(), spriteRender.color, TextureLibrary::Load(spriteRender.texture));
    // });

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
            renderable.shader_->SetUniform({ "lightSpaceMatrix", lightSpaceMatrix });
            renderable.shader_->SetUniform({ "shadowMap", int(15) });
            GLCALL(glActiveTexture(GL_TEXTURE0 + 15));
            GLCALL(glBindTexture(GL_TEXTURE_2D, m_shadow_fb->TextureID()));
        }
        Renderer::Draw(model);
    });

    // Render Renderables
    m_scene->ForEach<Renderable>([&](Entity ent, const Renderable& renderable) {
        renderable.shader_->SetUniform({ "view", m_perspectiveCam.GetViewMatrix() });
        renderable.shader_->SetUniform({ "projection", m_perspectiveCam.GetProjectionMatrix() });
        renderable.shader_->SetUniform({ "viewPos", m_perspectiveCam.Position() });
        renderable.shader_->SetUniform({ "model", ent.Get<TransformComponent>() });
        renderable.shader_->SetUniform({ "lightSpaceMatrix", lightSpaceMatrix });
        renderable.shader_->SetUniform({ "shadowMap", int(15) });
        GLCALL(glActiveTexture(GL_TEXTURE0 + 15));
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_shadow_fb->TextureID()));
        Renderer::Draw(renderable);
    });

    if (debug_config_.debug_view_ == DebugConfig::View::Shadow) {
        GLCALL(glDisable(GL_DEPTH_TEST));
        glm::mat4 quad_proj = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, -1.0f, 1.0f);
        quad_proj = glm::translate(quad_proj, { 3.0f, -3.0, 0.0 });
        auto* quad_shader = ShaderLibrary::Get("./res/shaders/simpleQuad.shader");
        quad_shader->Use();
        quad_shader->SetUniform(Uniform { .name = "depthMap", .value = int(0) });
        quad_shader->SetUniform(Uniform { .name = "view", .value = quad_proj });
        GLCALL(glActiveTexture(GL_TEXTURE0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_shadow_fb->TextureID()));
        m_quad.shader_ = quad_shader;
        Renderer::Draw(m_quad);
    }
}

void EngineImpl::UpdateWindow()
{
    m_window.SwapBuffers();
    m_window.PollEvents();
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

void EngineImpl::SetViewport(int width, int height)
{
    m_viewport_width = width;
    m_viewport_height = height;
    Renderer::SetViewport(width, height);
}

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
    auto basic_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("peanut.log", true);
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
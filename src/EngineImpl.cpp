#include "EngineImpl.hpp"

// peanut
#include "Pybind11.hpp"
#include "PythonBindings.hpp"
#include "Renderer/GLDebug.hpp" // TODO: Temporary
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "SceneSerializer.hpp"
#include "Settings.hpp"
#include "peanut/Application.hpp"
#include "peanut/DebugConfig.hpp"
#include "peanut/ModelLibrary.hpp"
#include "peanut/ShaderLibrary.hpp"

// external

// stl
#include <numeric>

// Forward decls

namespace spdlog {
void set_level(level::level_enum log_level);
}

namespace PEANUT {

namespace {
    auto ToSpdlogLevel(DebugConfig::LogLevel level) -> spdlog::level::level_enum
    {
        switch (level) {
        case DebugConfig::LogLevel::Error:
            return spdlog::level::level_enum::err;
        case DebugConfig::LogLevel::Warn:
            return spdlog::level::level_enum::warn;
        case DebugConfig::LogLevel::Info:
            return spdlog::level::level_enum::info;
        case DebugConfig::LogLevel::Debug:
            return spdlog::level::level_enum::debug;
        case DebugConfig::LogLevel::Trace:
            return spdlog::level::level_enum::trace;
        }
        assert(false);
        return spdlog::level::level_enum::info;
    }
}

EngineImpl::EngineImpl()
    : m_window("Peanut", 1280, 720)
    , m_scene(std::make_shared<Scene>())
    , shadow_tex_(Texture::Config { .type_ = Texture::Type::DEPTH,
          .width_ = 1024,
          .height_ = 1024,
          .wrapping_ = Texture::Wrapping::CLAMP_TO_BORDER,
          .filter_ = Texture::Filter::NEAREST,
          .border_color_ = { { 1.0f, 1.0f, 1.0f, 1.0f } } })
    , shadow_fb_({ &shadow_tex_ }, FrameBuffer::Config { .bind_draw_buf_ = false, .bind_read_buf_ = false })
    , m_quad(Renderable { .mesh_ = Renderer::GetQuadMesh(), .material_ = {}, .shader_ = nullptr })
    , m_viewport_width(0)
    , m_viewport_height(0)
{
    spdlog::set_level(spdlog::level::trace);

    m_window.SetEventCallback([this](Event& e) -> void { this->OnApplicationEvent(e); });
    Renderer2D::Init();
    Renderer::EnableDepthTest();

    LOG_DEBUG("Initializing Python Interpreter");
    pybind11::initialize_interpreter();
}

EngineImpl::~EngineImpl()
{
    m_scene->ForEach<PythonScriptComponent>(
        [](Entity, const PythonScriptComponent& comp) { pybind11::cast(comp.script_obj).dec_ref(); });
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
        spdlog::set_level(ToSpdlogLevel(debug_config_.log_level_));

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
    shadow_fb_.Bind();
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
    shadow_fb_.Unbind();

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, prev_fb));

    Renderer::SetViewport(m_window.GetWidth(), m_window.GetHeight());

    // Clear buffers for start of frame
    Renderer::ClearColor(0.2f, 0.2f, 0.2f, 0.1f);
    Renderer::ClearBuffers();

    // Render skybox
    m_scene->ForEach<SkyboxComponent>([&](Entity, const SkyboxComponent& skybox) { SkyboxSystem(skybox); });

    // Render Directional Lights
    m_scene->ForEach<DirectionalLightComponent>(
        [&](Entity, const DirectionalLightComponent& comp) { DirLightSystem(comp); });

    // Render Point Lights
    pointLights_.clear();
    m_scene->ForEach<PointLightComponent>(
        [&](Entity ent, const PointLightComponent& comp) { PointLightSystem(ent, comp); });
    // TODO: I don't like this
    Renderer::SetPointLights(pointLights_, *ShaderLibrary::Get("./res/shaders/Lighting.shader"));

    // Render Models
    m_scene->ForEach<ModelFileComponent>([&](Entity ent, const ModelFileComponent& comp) {
        auto& model = ModelLibrary::Get(comp.file);
        for (auto& renderable : model.GetRenderables()) {
            RenderableSystem(ent, renderable, lightSpaceMatrix);
        }
    });

    // Render Renderables
    m_scene->ForEach<Renderable>(
        [&](Entity ent, Renderable& renderable) { RenderableSystem(ent, renderable, lightSpaceMatrix); });

    // Shadow map debug view
    if (debug_config_.debug_view_ == DebugConfig::View::Shadow) {
        static const auto quad_proj
            = glm::translate(glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, -1.0f, 1.0f), { 3.0f, -3.0, 0.0 });
        static const auto quad_mesh = Renderer::GetQuadMesh();

        RenderCommand command;
        command.p_shader_ = ShaderLibrary::Get("./res/shaders/simpleQuad.shader");
        command.p_mesh_ = &quad_mesh;
        command.uniforms_.emplace_back("view", quad_proj);
        command.textures_.emplace_back("depthMap", &shadow_tex_);

        Renderer::DisableDepthTest();
        Renderer::Draw(command);
        Renderer::EnableDepthTest();
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
    auto inst = pybind11::cast(comp.script_obj);
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

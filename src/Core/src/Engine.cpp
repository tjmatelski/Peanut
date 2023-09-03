#include <Engine.hpp>

#include <Application.h>
#include <Events/WindowEvents.h>
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

#include <filesystem>
#include <memory>
#include <sol/sol.hpp>
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
            UpdateLuaScripts(timeStep);
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
            m_lightingShader->SetUniformVec3("viewPos", m_perspectiveCam.GetPosition());
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

void Engine::UpdateLuaScripts(double ts)
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.new_usertype<TransformComponent>("TransformComponent",
        "position", &TransformComponent::translation,
        "rotation", &TransformComponent::rotation,
        "scale", &TransformComponent::scale);

    lua.new_usertype<glm::vec3>("glm::vec3",
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z);

    lua.new_enum("KeyCode",
        "SPACE", KeyCode::SPACE,
        "APOSTROPHE", KeyCode::APOSTROPHE,
        "COMMA", KeyCode::COMMA,
        "MINUS", KeyCode::MINUS,
        "PERIOD", KeyCode::PERIOD,
        "SLASH", KeyCode::SLASH,
        "ZERO", KeyCode::ZERO,
        "ONE", KeyCode::ONE,
        "TWO", KeyCode::TWO,
        "THREE", KeyCode::THREE,
        "FOUR", KeyCode::FOUR,
        "FIVE", KeyCode::FIVE,
        "SIX", KeyCode::SIX,
        "SEVEN", KeyCode::SEVEN,
        "EIGHT", KeyCode::EIGHT,
        "NINE", KeyCode::NINE,
        "SEMICOLON", KeyCode::SEMICOLON,
        "EQUAL", KeyCode::EQUAL,
        "A", KeyCode::A,
        "B", KeyCode::B,
        "C", KeyCode::C,
        "D", KeyCode::D,
        "E", KeyCode::E,
        "F", KeyCode::F,
        "G", KeyCode::G,
        "H", KeyCode::H,
        "I", KeyCode::I,
        "J", KeyCode::J,
        "K", KeyCode::K,
        "L", KeyCode::L,
        "M", KeyCode::M,
        "N", KeyCode::N,
        "O", KeyCode::O,
        "P", KeyCode::P,
        "Q", KeyCode::Q,
        "R", KeyCode::R,
        "S", KeyCode::S,
        "T", KeyCode::T,
        "U", KeyCode::U,
        "V", KeyCode::V,
        "W", KeyCode::W,
        "X", KeyCode::X,
        "Y", KeyCode::Y,
        "Z", KeyCode::Z,
        "ESCAPE", KeyCode::ESCAPE,
        "ENTER", KeyCode::ENTER,
        "TAB", KeyCode::TAB,
        "BACKSPACE", KeyCode::BACKSPACE,
        "INSERT", KeyCode::INSERT,
        "DELETE", KeyCode::DELETE,
        "RIGHT", KeyCode::RIGHT,
        "LEFT", KeyCode::LEFT,
        "DOWN", KeyCode::DOWN,
        "UP", KeyCode::UP,
        "PAGE_UP", KeyCode::PAGE_UP,
        "PAGE_DOWN", KeyCode::PAGE_DOWN,
        "HOME", KeyCode::HOME,
        "END", KeyCode::END,
        "CAPS_LOCK", KeyCode::CAPS_LOCK,
        "SCROLL_LOCK", KeyCode::SCROLL_LOCK,
        "NUM_LOCK", KeyCode::NUM_LOCK,
        "PRINT_SCREEN", KeyCode::PRINT_SCREEN,
        "PAUSE", KeyCode::PAUSE,
        "F1", KeyCode::F1,
        "F2", KeyCode::F2,
        "F3", KeyCode::F3,
        "F4", KeyCode::F4,
        "F5", KeyCode::F5,
        "F6", KeyCode::F6,
        "F7", KeyCode::F7,
        "F8", KeyCode::F8,
        "F9", KeyCode::F9,
        "F10", KeyCode::F10,
        "F11", KeyCode::F11,
        "F12", KeyCode::F12,
        "F13", KeyCode::F13,
        "F14", KeyCode::F14,
        "F15", KeyCode::F15,
        "F16", KeyCode::F16,
        "F17", KeyCode::F17,
        "F18", KeyCode::F18,
        "F19", KeyCode::F19,
        "F20", KeyCode::F20,
        "F21", KeyCode::F21,
        "F22", KeyCode::F22,
        "F23", KeyCode::F23,
        "F24", KeyCode::F24,
        "F25", KeyCode::F25,
        "KP_0", KeyCode::KP_0,
        "KP_1", KeyCode::KP_1,
        "KP_2", KeyCode::KP_2,
        "KP_3", KeyCode::KP_3,
        "KP_4", KeyCode::KP_4,
        "KP_5", KeyCode::KP_5,
        "KP_6", KeyCode::KP_6,
        "KP_7", KeyCode::KP_7,
        "KP_8", KeyCode::KP_8,
        "KP_9", KeyCode::KP_9,
        "KP_DECIMAL", KeyCode::KP_DECIMAL,
        "KP_DIVIDE", KeyCode::KP_DIVIDE,
        "KP_MULTIPLY", KeyCode::KP_MULTIPLY,
        "KP_SUBTRACT", KeyCode::KP_SUBTRACT,
        "KP_ADD", KeyCode::KP_ADD,
        "KP_ENTER", KeyCode::KP_ENTER,
        "KP_EQUAL", KeyCode::KP_EQUAL,
        "LEFT_SHIFT", KeyCode::LEFT_SHIFT,
        "LEFT_CONTROL", KeyCode::LEFT_CONTROL,
        "LEFT_ALT", KeyCode::LEFT_ALT,
        "LEFT_SUPER", KeyCode::LEFT_SUPER,
        "RIGHT_SHIFT", KeyCode::RIGHT_SHIFT,
        "RIGHT_CONTROL", KeyCode::RIGHT_CONTROL,
        "RIGHT_ALT", KeyCode::RIGHT_ALT,
        "RIGHT_SUPER", KeyCode::RIGHT_SUPER,
        "MENU", KeyCode::MENU);

    lua.new_enum("MouseCode",
        "MOUSE_BUTTON_LEFT", MouseCode::MOUSE_BUTTON_LEFT,
        "MOUSE_BUTTON_RIGHT", MouseCode::MOUSE_BUTTON_RIGHT,
        "MOUSE_BUTTON_MIDDLE", MouseCode::MOUSE_BUTTON_MIDDLE,
        "MOUSE_BUTTON_4", MouseCode::MOUSE_BUTTON_4,
        "MOUSE_BUTTON_5", MouseCode::MOUSE_BUTTON_5,
        "MOUSE_BUTTON_6", MouseCode::MOUSE_BUTTON_6,
        "MOUSE_BUTTON_7", MouseCode::MOUSE_BUTTON_7,
        "MOUSE_BUTTON_8", MouseCode::MOUSE_BUTTON_8);

    m_scene->ForEachEntity([&](Entity ent) {
        if (ent.Has<LuaScriptComponent>()) {
            lua["transform"] = &ent.Get<TransformComponent>();
            lua["IsKeyPressed"] = [=](KeyCode kc) {
                return m_window->IsKeyPressed(kc);
            };
            const auto& scriptComp = ent.Get<LuaScriptComponent>();
            lua.script_file(std::filesystem::absolute(scriptComp.script));
            lua["OnUpdate"](ts);
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
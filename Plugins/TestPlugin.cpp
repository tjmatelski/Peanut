#include "Input/Input.h"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/NativeScript.h"
#include "Scene/Scene.h"
#include "Utils/Log.h"

#include <glm/fwd.hpp>
#include <iostream>
#include <linux/limits.h>
#include <memory>

extern "C" const char* plugin_name()
{
    return "Test Plugin";
}

class TestPlugin : public PEANUT::NativeScript {
public:
    bool my_bool = false;
    float my_float = 0.0f;
    double speed = 0.0;
    int my_int = 0;
    std::string my_str = "";

    TestPlugin()
    {
        m_members.emplace_back(PEANUT::MemberVariable { "my_bool", PEANUT::MemberVariable::Type::Bool, &my_bool });
        m_members.emplace_back(PEANUT::MemberVariable { "my_float", PEANUT::MemberVariable::Type::Float, &my_float });
        m_members.emplace_back(PEANUT::MemberVariable { "speed", PEANUT::MemberVariable::Type::Double, &speed });
        m_members.emplace_back(PEANUT::MemberVariable { "my_int", PEANUT::MemberVariable::Type::Int, &my_int });
        m_members.emplace_back(PEANUT::MemberVariable { "my_str", PEANUT::MemberVariable::Type::String, &my_str });
    }

    TestPlugin(const TestPlugin& other) = delete;

    void OnCreate() override
    {
        using namespace PEANUT;
        auto& model = m_ent.Has<CustomModelComponent>() ? m_ent.Get<CustomModelComponent>() : m_ent.Add<CustomModelComponent>();
        model.mesh.indices.clear();
        model.mesh.vertices.clear();

        model.mesh.vertices.emplace_back(Vertex { glm::vec3 { 1, 0, 0 }, glm::vec3 { 0, 0, -1 }, glm::vec2 { 1, 0 } });
        model.mesh.vertices.emplace_back(Vertex { glm::vec3 { 0, 1, 0 }, glm::vec3 { 0, 0, -1 }, glm::vec2 { 0, 1 } });
        model.mesh.vertices.emplace_back(Vertex { glm::vec3 { 1, 1, 0 }, glm::vec3 { 0, 0, -1 }, glm::vec2 { 1, 1 } });
        model.mesh.indices.emplace_back(0);
        model.mesh.indices.emplace_back(1);
        model.mesh.indices.emplace_back(2);

        LOG_DEBUG("TestPlugin created");
    }

    void OnDestroy() override
    {
        LOG_DEBUG("TestPlugin Destroyed");
    }

    void RuntimeBegin() override
    {
        LOG_DEBUG("TestPlugin runtime begin");
    }

    void RuntimeEnd() override
    {
        LOG_DEBUG("TestPlugin runtime end");
    }

    void EditorUpdate() override
    {
        if (my_bool) {
            LOG_DEBUG("TestPlugin my_bool true");
        }
    }

    void Update(double dt) override
    {
        if (PEANUT::Input::IsKeyPressed(PEANUT::KeyCode::W)) {
            m_ent.Get<PEANUT::TransformComponent>().translation.z += speed * dt;
        }
        if (PEANUT::Input::IsKeyPressed(PEANUT::KeyCode::A)) {
            m_ent.Get<PEANUT::TransformComponent>().translation.x += speed * dt;
        }
        if (PEANUT::Input::IsKeyPressed(PEANUT::KeyCode::S)) {
            m_ent.Get<PEANUT::TransformComponent>().translation.z -= speed * dt;
        }
        if (PEANUT::Input::IsKeyPressed(PEANUT::KeyCode::D)) {
            m_ent.Get<PEANUT::TransformComponent>().translation.x -= speed * dt;
        }
    }
};

extern "C" PEANUT::NativeScript* get_new_component()
{
    LOG_DEBUG("Creating new TestComponent");
    return new TestPlugin {};
}

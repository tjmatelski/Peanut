#include "Scene/Entity.h"
#include "Scene/NativeScript.h"
#include "Scene/Scene.h"
#include "Utils/Log.h"

#include <iostream>
#include <linux/limits.h>
#include <memory>

extern "C" const char* plugin_name()
{
    return "Test Plugin";
}

class TestPlugin : public PEANUT::NativeScript {
public:
    float speed = 0.0f;
    int test_int = 0;
    std::string string = "";

    TestPlugin()
    {
        PEANUT::MemberVariable member;
        member.name = "speed";
        member.type = PEANUT::MemberVariable::Type::Float;
        member.addr = &speed;
        m_members.push_back(member);
        member.name = "test_int";
        member.type = PEANUT::MemberVariable::Type::Int;
        member.addr = &test_int;
        m_members.push_back(member);
        member.name = "string";
        member.type = PEANUT::MemberVariable::Type::String;
        member.addr = &string;
        m_members.push_back(member);
    }

    TestPlugin(const TestPlugin& other) = delete;

    void Update(double dt)
    {
        LOG_DEBUG("Updating TestPlugin");
    }
};

extern "C" PEANUT::NativeScript* get_new_component()
{
    LOG_DEBUG("Creating new TestComponent");
    return new TestPlugin {};
}

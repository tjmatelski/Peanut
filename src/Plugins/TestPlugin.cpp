#include "Scene/Entity.h"
#include "Scene/NativeScript.h"

#include <iostream>
#include <linux/limits.h>

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
};

extern "C" void add_to_entity(PEANUT::Entity ent)
{
    std::cout << "Adding Test Plugin" << std::endl;
    ent.Add<TestPlugin>();
}

extern "C" void remove_from_entity(PEANUT::Entity ent)
{
    std::cout << "Removing Test Plugin. speed = " << ent.Get<TestPlugin>().speed << std::endl;
    ent.Remove<TestPlugin>();
}

extern "C" bool entity_has_component(PEANUT::Entity ent)
{
    return ent.Has<TestPlugin>();
}

extern "C" PEANUT::NativeScript* get_as_native_script(PEANUT::Entity ent)
{
    return &ent.Get<TestPlugin>();
}

#include "Scene/Entity.h"
#include "Scene/NativeScript.h"

#include <iostream>

extern "C" const char* plugin_name()
{
    return "Test Plugin";
}

class TestPlugin : public PEANUT::NativeScript {
};

extern "C" void add_to_entity(PEANUT::Entity ent)
{
    ent.Add<TestPlugin>();
    std::cout << "TEST PLUGIN" << std::endl;
}
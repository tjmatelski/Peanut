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
    std::cout << "Adding Test Plugin" << std::endl;
    ent.Add<TestPlugin>();
}

extern "C" void remove_from_entity(PEANUT::Entity ent)
{
    std::cout << "Removing Test Plugin" << std::endl;
    ent.Remove<TestPlugin>();
}

extern "C" bool entity_has_component(PEANUT::Entity ent)
{
    return ent.Has<TestPlugin>();
}

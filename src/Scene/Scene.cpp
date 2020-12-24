#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Component.h>

namespace PEANUT
{

Entity Scene::CreateEntity(const std::string& name)
{
    Entity entity(m_registry.create(), this);
    entity.Add<TagComponent>(name);
    return entity;
}

}
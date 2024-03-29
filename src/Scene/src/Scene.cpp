#include <Scene/Component.h>
#include <Scene/Entity.h>
#include <Scene/Scene.h>

namespace PEANUT {

void Scene::Clear()
{
    m_registry.clear();
}

Entity Scene::CreateEntity(const std::string& name)
{
    Entity entity(m_registry.create(), this);
    entity.Add<TagComponent>(name);
    entity.Add<TransformComponent>();
    return entity;
}

}
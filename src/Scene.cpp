#include <peanut/Scene.hpp>

#include <peanut/Component.hpp>
#include <peanut/Entity.hpp>

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
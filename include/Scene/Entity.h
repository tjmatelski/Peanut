#pragma once


#include "Scene.h"

namespace PEANUT
{
    
class Entity
{
public:
    Entity() = default;
    Entity(const entt::entity entity, Scene* scene);
    ~Entity() = default;

    template <typename T, typename... Args>
    T& Add(Args&&... args)
    {
        T& component = m_scene->m_registry.emplace<T>(m_entityID, args...);
        return component;
    }

private:
    entt::entity m_entityID;
    Scene* m_scene;
};

} // namespace PEANUT
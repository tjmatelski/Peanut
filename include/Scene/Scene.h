#pragma once

#include <entt.hpp>
#include "../Log.h"

namespace PEANUT
{

class Entity;

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    void Clear();
    Entity CreateEntity(const std::string& name = "Default Entity Name");
    template <typename Functor>
    void ForEachEntity(Functor func)
    {
        m_registry.each([&](entt::entity ent){ func(Entity(ent, this));});
    }

private:
    entt::registry m_registry;
    friend class Entity;
};
}
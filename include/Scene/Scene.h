#pragma once

#include <entt.hpp>

namespace PEANUT
{

class Entity;

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    Entity CreateEntity(const std::string& name = "Default Entity Name");

private:
    entt::registry m_registry;
    friend class Entity;
};
}
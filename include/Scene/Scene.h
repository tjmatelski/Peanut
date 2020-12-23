#pragma once

#include <entt.hpp>

namespace PEANUT
{

class Scene
{
public:
    Scene() = default;

private:
    entt::registry m_registry;
};
}
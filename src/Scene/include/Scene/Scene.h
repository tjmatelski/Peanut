#pragma once

#include <entt/entt.hpp>

namespace PEANUT {

class Entity;

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void Clear();
    Entity CreateEntity(const std::string& name = "Default Entity Name");

    template <typename Functor>
    void ForEachEntity(Functor func)
    {
        m_registry.each([&](entt::entity ent) { func({ ent, this }); });
    }

    template <class Comp, class Fn>
    void ForEach(Fn func)
    {
        auto view = m_registry.view<Comp>();
        view.each([&](auto ent, auto comp) { func({ ent, this }, comp); });
    }

private:
    entt::registry m_registry;
    friend class Entity;
};
}
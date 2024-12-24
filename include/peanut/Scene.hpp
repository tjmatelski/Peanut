#pragma once

// external
#include <entt/entt.hpp>

// stl
#include <type_traits>

namespace PEANUT {

class Entity;

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void Clear();
    Entity CreateEntity(const std::string& name = "Default Entity Name");

    template <class Functor>
    void ForEachEntity(Functor func)
    {
        m_registry.each([&](entt::entity ent) { func({ ent, this }); });
    }

    template <class... Args, class Fn>
    void ForEach(Fn func)
    {
        auto view = m_registry.view<std::remove_reference_t<Args>...>();
        view.each([&](auto ent, Args&... args) { func({ ent, this }, std::forward<Args...>(args)...); });
    }

private:
    entt::registry m_registry;
    friend class Entity;
};
}
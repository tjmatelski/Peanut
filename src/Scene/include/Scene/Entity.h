#pragma once

#include "Scene.h"
#include "Scene/NativeScript.h"
#include <entt/core/hashed_string.hpp>
#include <memory>
#include <string_view>

namespace PEANUT {

class Entity {

    decltype(auto) GetStorage(const std::string& name)
    {
        return m_scene->m_registry.storage<std::unique_ptr<NativeScript>>(entt::hashed_string { name.c_str() });
    }

    NativeScript* AddScript(const std::string& name, NativeScript* comp)
    {
        return GetStorage(name).emplace(m_entityID, comp).get();
    }

public:
    Entity() = default;
    Entity(const entt::entity entity, Scene* scene);
    ~Entity() = default;

    operator bool() const { return (m_entityID != entt::null) && m_scene; }

    template <typename T, typename... Args>
    decltype(auto) Add(Args&&... args)
    {
        if constexpr (std::is_same_v<T, NativeScript>) {
            return AddScript(std::forward<Args>(args)...);
        } else {
            return m_scene->m_registry.emplace<T>(m_entityID, std::forward<Args>(args)...);
        }
    }

    template <typename T, class... Args>
    void Remove(Args&&... args)
    {
        if constexpr (std::is_same_v<T, NativeScript>) {
            GetStorage(std::forward<Args>(args)...).remove(m_entityID);
        } else {
            m_scene->m_registry.remove<T>(m_entityID);
        }
    }

    template <typename T, class... Args>
    decltype(auto) Get(Args&&... args)
    {
        if constexpr (std::is_same_v<T, NativeScript>) {
            return GetStorage(std::forward<Args>(args)...).get(m_entityID).get();
        } else {
            return m_scene->m_registry.get<T>(m_entityID);
        }
    }

    template <typename T, class... Args>
    bool Has(Args&&... args)
    {
        if constexpr (std::is_same_v<T, NativeScript>) {
            return GetStorage(std::forward<Args>(args)...).contains(m_entityID);
        } else {
            return m_scene->m_registry.any_of<T>(m_entityID);
        }
    }

private:
    entt::entity m_entityID;
    Scene* m_scene;
};

} // namespace PEANUT
#pragma once

#include "Scene.h"
#include <entt/core/hashed_string.hpp>
#include <memory>
#include <string_view>

namespace PEANUT {

class NativeScript;

class Entity {

    entt::storage_for_t<std::unique_ptr<PEANUT::NativeScript, std::default_delete<PEANUT::NativeScript>>>& GetStorage(const std::string& name);
    NativeScript* AddScript(const std::string& name, NativeScript* comp);

public:
    Entity();
    Entity(const entt::entity entity, Scene* scene);
    ~Entity();

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
    Scene* m_scene = nullptr;
};

} // namespace PEANUT
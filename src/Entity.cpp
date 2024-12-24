#include <peanut/Entity.hpp>

#include <peanut/NativeScript.hpp>

namespace PEANUT {

Entity::Entity() = default;
Entity::~Entity() = default;

Entity::Entity(const entt::entity entity, Scene* scene)
    : m_entityID(entity)
    , m_scene(scene)
{
}

NativeScript* Entity::AddScript(const std::string& name, NativeScript* comp)
{
    auto& script = GetStorage(name).emplace(m_entityID, comp);
    script->m_ent = *this;
    return script.get();
}

entt::storage_for_t<std::unique_ptr<PEANUT::NativeScript, std::default_delete<PEANUT::NativeScript>>>& Entity::GetStorage(const std::string& name)
{
    return m_scene->m_registry.storage<std::unique_ptr<NativeScript>>(entt::hashed_string { name.c_str() });
}

} // namespace PEANUT
#include <Scene/Entity.h>

namespace PEANUT
{

    Entity::Entity(const entt::entity entity, Scene *scene) : m_entityID(entity), m_scene(scene) {}

} // namespace PEANUT
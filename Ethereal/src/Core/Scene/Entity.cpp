#include "pch.h"
#include "Entity.h"

namespace Ethereal
{
    Entity::Entity(entt::entity handle, Scene* Scene) : m_Entity(handle), m_Scene(Scene) {}
}  // namespace Ethereal
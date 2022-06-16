#include "Entity.hpp"

namespace Engine
{
    Entity::Entity(entt::entity handle, Scene* Scene)
    :m_Entity(handle),m_Scene(Scene)
    {
        
    }
}
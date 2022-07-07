#include "GameObject.hpp"

namespace Ethereal
{
    void GameObject::AddRenderEntity(RenderEntity renderEntity)
    {
        m_RenderEntities.push_back(renderEntity);
    }
}